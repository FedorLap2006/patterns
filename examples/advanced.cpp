#include <patterns/Visitor.hpp>
#include <memory>
#include <vector>
#include <iostream>

using namespace std;
using namespace patterns;

const int nodeANumber = 1;
const int nodeBNumber = 10;
const int compositorNumber = 100;
const int treeNumber = 1000;

class Node : public BaseVisitable<VisitorTraits<int, Node>> {
public:
    MAKE_VISITABLE()

    virtual int number() const = 0;
};

class NodeA : public Node {
public:
    MAKE_VISITABLE()

    int number() const override {
        cout << "A am NodeA. My number is " << nodeANumber << endl;
        return nodeANumber;
    }
};

class NodeB : public Node {
public:
    MAKE_VISITABLE()

    int number() const override {
        cout << "A am NodeB. My number is " << nodeBNumber << endl;
        return nodeBNumber;
    }
};

class Compositor : public Node {
public:
    // Переопределение стандартного поведения accept для автоматического посещения мемберов.
    MAKE_VISITABLE_CUSTOM({
        return pvisitor->visit(*this) + mLhs->accept(visitor) + mRhs->accept(visitor);
    })

    explicit Compositor(shared_ptr<Node> lhs, shared_ptr<Node> rhs) :
            mLhs(move(lhs)), mRhs(move(rhs)) {
    }

    int number() const override {
        cout << "A am Compositor. My number is " << compositorNumber << endl;
        return compositorNumber;
    }

private:
    shared_ptr<Node> mLhs, mRhs;
};

class Tree : public Node {
public:
    // Переопределение стандартного поведения accept для автоматического посещения мемберов.
    MAKE_VISITABLE_CUSTOM({
        int total = pvisitor->visit(*this);

        for (auto const &node : mNodes) {
            total += node->accept(visitor);
        }

        return total;
    })

    void addNode(shared_ptr<Node> node) {
        mNodes.push_back(move(node));
    }

    int number() const override {
        cout << "A am Tree. My number is " << treeNumber << endl;
        return treeNumber;
    }

private:
    vector<shared_ptr<Node>> mNodes;
};

//                                                     |Node| - тип перехватчик
class MyVisitor : public BaseVisitor<VisitorTraits<int, Node>, Node, NodeA> {
public:
    // Перехватывает все непринятые типы, производные от Node
    int visit(Node &node) override {
        return node.number();
    }

    // Специфичное поведжения для NodeA
    int visit(NodeA &node) override {
        cout << "NodeA number in square" << endl;
        return node.number() * node.number();
    }
};

int main(int argc, char **argv) {
    MyVisitor visitor;
    shared_ptr<Node> nodeA = make_shared<NodeA>();
    shared_ptr<Node> nodeB = make_shared<NodeB>();
    shared_ptr<Node> compositor = make_shared<Compositor>(nodeA, nodeB);
    shared_ptr<Tree> ptree = make_shared<Tree>();
    ptree->addNode(nodeA);
    ptree->addNode(compositor);
    ptree->addNode(nodeB);

    shared_ptr<Node> tree = ptree;
    ptree->accept(visitor);
}