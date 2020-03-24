#pragma once

#include <stdexcept>
#include <type_traits>

namespace patterns {
    // Не интересные для пользователя детали реализации.
    namespace util {
        template<typename R>
        class BasicVisitor {
        public:
            explicit BasicVisitor() = default;

            virtual ~BasicVisitor() = default;

            BasicVisitor(BasicVisitor const &) = delete;

            BasicVisitor(BasicVisitor &&) = delete;

            BasicVisitor &operator=(BasicVisitor const &) = delete;

            BasicVisitor &operator=(BasicVisitor &&) = delete;

            virtual R catchInvalid() {
                throw std::runtime_error(
                        "cannot accept visitor without implement of 'visit' method for the type");
            }
        };

        template<typename T, typename R = void>
        class VisitorImpl {
        public:
            virtual ~VisitorImpl() = default;

            virtual R visit(T &) = 0;
        };
    }

    // Наследуйте свой собственный визитор от этого класса, передавая первым аргументом шаблона
    // класс свойтв, а следующими аргументами все типы, которые вы будете посещать
    template<typename TraitsT, typename... Types>
    class BaseVisitor :
            public util::BasicVisitor<typename TraitsT::ReturnType>,
            public util::VisitorImpl<Types, typename TraitsT::ReturnType> ... {
    };

    // Наследуйте свой базовый посещаемый тип от этого класса, передавая теже свойста, что и
    // своему визитору.
    //
    // ОБЯЗАТЕЛЬНО ВЫЗОВИТЕ МАКРОС MAKE_VISITABLE_CUSTOM или MAKE_VISITABLE !!!
    //
    template<typename TraitsT>
    class BaseVisitable {
    public:
        using Traits = TraitsT;
        using ReturnType = typename TraitsT::ReturnType;
        using CatchType = typename TraitsT::CatchType;

        explicit BaseVisitable() = default;

        virtual ~BaseVisitable() = default;

        BaseVisitable(BaseVisitable const &) = delete;

        BaseVisitable(BaseVisitable &&) = delete;

        BaseVisitable &operator=(BaseVisitable const &) = delete;

        BaseVisitable &operator=(BaseVisitable &&) = delete;

        virtual ReturnType accept(util::BasicVisitor<ReturnType> &visitor) = 0;

    protected:
        template<typename T, typename PT>
        static ReturnType
        acceptImpl(T &visited, util::BasicVisitor<ReturnType> &visitor, PT const &predicate) {
            if (auto p = dynamic_cast<patterns::util::VisitorImpl<T, ReturnType> *>(&visitor)) {
                return predicate(p, visitor);
            }
            if (auto p =
                    dynamic_cast<patterns::util::VisitorImpl<CatchType, ReturnType> *>(&visitor)) {
                return predicate(p, visitor);
            }
            return visitor.catchInvalid();
        }
    };

    // Кастомный тип свойств.
    // ReturnType - возвращаемое значение всех функций visit и apply.
    // CatchType - Тип перехватчик всех непринятых типов.
    // Пример :
    //        class MyVisitor : public BaseVisitor<VisitorTraits<int, Node>, Node, NodeA> {
    //        public:
    //          // Посещает все объекты производные от класса Node, так как Node в VisitorTraits
    //          // Указан как тип перехватчик.
    //          int visit(Node &node) override {
    //              return node.number();
    //          }
    //
    //          // Посещает все объекты динамического типа NodeA.
    //          int visit(NodeA &node) override {
    //              return node.number() * node.number();
    //          }
    //       };
    //
    template<typename R, typename C = void>
    struct VisitorTraits {
        using ReturnType = R;
        using CatchType = C;
    };

    // Специализация для void, означающая, что в качестве перехватчика будет использоваться самый
    // Базовый тип BaseVisitable.
    template<typename R>
    struct VisitorTraits<R, void> {
        using ReturnType = R;
        using CatchType = BaseVisitable<VisitorTraits<R>>;
    };

    // Стандартные BaseVisitor и BaseVisitable;

    template<typename... Types> using Visitor = BaseVisitor<VisitorTraits<void>, Types...>;
    using Visitable = BaseVisitable<VisitorTraits<void>>;
}

// Используется для кастомной настройки вызова функции accept
// pvisitor - тип, уже приведённый до VisitorImpl<ВашТип>. Используйте для pvisitor->visit(*this).
// visitor - базовый тип визитора BasicVisitor. Используйте для вызова apply для других объектов.
// Пример :
//    class Compositor : public Node {
//    public:
//        MAKE_VISITABLE_CUSTOM({
//            return pvisitor->visit(*this) + mLhs->accept(visitor) + mRhs->accept(visitor);
//        })
//    private:
//        shared_ptr<Node> mLhs, mRhs;
//    };
// pvisitor->visit(*this) посещает самого себя.
// mLhs->accept(visitor) переадрисует посещение дочерним объектам
//
// НЕ ЗАБУДЬТЕ ВЕРНУТЬ ЗНАЧЕНИЕ, ЕСЛИ ТИП ВОЗВРАЩАЕМОГО  ЗНАЧЕНЯИ ОТЛИЧЕН ОТ void !!!
//
#define MAKE_VISITABLE_CUSTOM(...) \
    virtual ReturnType accept(patterns::util::BasicVisitor<ReturnType>& visitor) override \
    { return acceptImpl(*this, visitor, [this](auto* pvisitor, auto& visitor) __VA_ARGS__ ); }

// Стандартная реализация MAKE_VISITABLE_CUSTOM для посещения самого себя.
#define MAKE_VISITABLE() \
    MAKE_VISITABLE_CUSTOM({ return pvisitor->visit(*this); })
