#include "../includes/evaluator.h"

ExprNode *Evaluator::Eval(ExprNode *node)
{
    return node->Eval(this);
}

ExprNode *Evaluator::Eval(PostfixDecrementNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(PrefixDecrementNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(PostfixIncrementNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(PrefixIncrementNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(ArrayAccessNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(BinOpNode *node)
{
    auto left = Eval(node->Left()), right = Eval(node->Right());
    if (!left || !right) return nullptr;
    auto ileft = dynamic_cast<IntConstNode *>(left), iright = dynamic_cast<IntConstNode *>(right);
    auto fleft = dynamic_cast<FloatConstNode *>(left), fright = dynamic_cast<FloatConstNode *>(right);
    std::shared_ptr<Token> t;
    double value = 0;
#define EVAL(OP)\
    if (ileft && iright)\
    {\
        value = ileft->GetValue() OP iright->GetValue();\
        t = std::make_shared<Token>(TokenType::NUM_INT, -1, -1, std::to_string((int)value));\
        t->intValue = value;\
        return new IntConstNode(t);\
    }

    switch (node->GetOperation()->type)
    {
        case TokenType::PLUS:
            if (ileft && iright)
            {
                value = ileft->GetValue() + iright->GetValue();
                t = std::make_shared<Token>(TokenType::NUM_INT, -1, -1, std::to_string((int)value));
                t->intValue = value;
                return new IntConstNode(t);
            }
        case TokenType::MINUS:
            if (ileft && iright)
            {
                value = ileft->GetValue() - iright->GetValue();
                t = std::make_shared<Token>(TokenType::NUM_INT, -1, -1, std::to_string((int)value));
                t->intValue = value;
                return new IntConstNode(t);
            }
        case TokenType::ASTERIX:
            if (ileft && iright)
            {
                value = ileft->GetValue() * iright->GetValue();
                t = std::make_shared<Token>(TokenType::NUM_INT, -1, -1, std::to_string((int)value));
                t->intValue = value;
                return new IntConstNode(t);
            }
        case TokenType::BITWISE_AND:
            EVAL(&);
        case TokenType::BITWISE_OR:
            EVAL(|)

    }
    return nullptr;
}

ExprNode *Evaluator::Eval(UnaryOpNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(SizeofExprNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(SizeofTypeNameNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(AssignmentNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(IntConstNode *node)
{
    return node;
}

ExprNode *Evaluator::Eval(FloatConstNode *node)
{
    return node;
}

ExprNode *Evaluator::Eval(TypeCastNode *node)
{
    return nullptr;
}

ExprNode *Evaluator::Eval(EnumeratorNode *node)
{
    return node->GetValue();
}
