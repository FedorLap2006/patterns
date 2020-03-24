#include <patterns/Visitor.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace patterns;

class Node : public Visitable {
public:
    MAKE_VISITABLE()

    virtual void print() const = 0;
};

class NodeA : public Node {
public:
    MAKE_VISITABLE()

    void print() const override {
        cout << "I am NodeA!" << endl;
    }

    int numBananas() const {
        return 30;
    }
};

class NodeB : public Node {
public:
    MAKE_VISITABLE()

    void print() const override {
        cout << "I am NodeB!" << endl;
    }

    string sentiment() const {
        return "happy";
    }
};

class Compositor : public Node {
public:
    MAKE_VISITABLE()

    explicit Compositor(shared_ptr<Node> lhs, shared_ptr<Node> rhs) :
            mLhs(move(lhs)), mRhs(move(rhs)) {
    }

    void print() const override {
        cout << "I am Compositor!" << endl;
    }

    shared_ptr<Node> lhs() const {
        return mLhs;
    }

    shared_ptr<Node> rhs() const {
        return mRhs;
    }

private:
    shared_ptr<Node> mLhs, mRhs;
};

class MyVisitor : public Visitor<NodeA, NodeB, Compositor> {
public:
    void visit(NodeA &node) override {
        printNode(node);
        printDepth();
        cout << "\tI have a " << node.numBananas() << " bananas !!!\n";
    }

    void visit(NodeB &node) override {
        printNode(node);
        printDepth();
        cout << "\tI " << node.sentiment() << "!\n";
    }

    void visit(Compositor &node) override {
        printNode(node);
        ++visitDepth;
        node.lhs()->accept(*this);
        node.rhs()->accept(*this);
        --visitDepth;
    }

private:
    void printDepth() const {
        for (std::size_t i = 0; i != visitDepth; ++i) {
            cout << "\t";
        }
    }

    void printNode(Node &node) {
        printDepth();
        node.print();
    }

    size_t visitDepth = 0;
};

int main() {
    shared_ptr<Node> nodeA = make_shared<NodeA>();
    shared_ptr<Node> nodeB = make_shared<NodeB>();
    shared_ptr<Node> compositor = make_shared<Compositor>(nodeA, nodeB);
    shared_ptr<Node> compositor2 = make_shared<Compositor>(compositor, nodeA);

    MyVisitor visitor;
    compositor2->accept(visitor);
};