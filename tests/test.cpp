#include <patterns/Visitor.hpp>
#include <memory>
#include <gtest/gtest.h>

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
        return nodeANumber;
    }
};

class NodeB : public Node {
public:
    MAKE_VISITABLE()

    int number() const override {
        return nodeBNumber;
    }
};

class Compositor : public Node {
public:
    MAKE_VISITABLE_CUSTOM({
        return pvisitor->visit(*this) + mLhs->accept(visitor) + mRhs->accept(visitor);
    })

    explicit Compositor(shared_ptr<Node> lhs, shared_ptr<Node> rhs) :
            mLhs(move(lhs)), mRhs(move(rhs)) {
    }

    int number() const override {
        return compositorNumber;
    }

private:
    shared_ptr<Node> mLhs, mRhs;
};

class Tree : public Node {
public:
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
        return treeNumber;
    }

private:
    vector<shared_ptr<Node>> mNodes;
};

class MyVisitor : public BaseVisitor<VisitorTraits<int, Node>, Node, NodeA> {
public:
    int visit(Node &node) override {
        return node.number();
    }

    int visit(NodeA &node) override {
        return node.number() * node.number();
    }
};

class TestVisitor : public ::testing::Test {
protected:
    MyVisitor visitor;
    shared_ptr<Node> nodeA = make_shared<NodeA>();
    shared_ptr<Node> nodeB = make_shared<NodeB>();
    shared_ptr<Node> compositor = make_shared<Compositor>(nodeA, nodeB);
};

TEST_F(TestVisitor, TestInit) {
    ASSERT_EQ(nodeA->number(), nodeANumber);
    ASSERT_EQ(nodeB->number(), nodeBNumber);
    ASSERT_EQ(compositor->number(), compositorNumber);
}

TEST_F(TestVisitor, TestVisit) {
    ASSERT_EQ(nodeA->accept(visitor), nodeANumber * nodeANumber);
    ASSERT_EQ(nodeB->accept(visitor), nodeBNumber);
    ASSERT_EQ(compositor->accept(visitor),
            compositorNumber + nodeANumber * nodeANumber + nodeBNumber);
}

TEST_F(TestVisitor, TestTree) {
    auto tree = make_shared<Tree>();
    tree->addNode(nodeA);
    tree->addNode(nodeB);
    tree->addNode(compositor);

    ASSERT_EQ(tree->accept(visitor),
            treeNumber + nodeANumber * nodeANumber + nodeBNumber + nodeANumber * nodeANumber +
                    nodeBNumber + compositorNumber);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}