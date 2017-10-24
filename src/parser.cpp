//
// Created by keltar on 10/14/17.
//

#include "../includes/parser.h"

Parser::Parser() = default;

Parser::Parser(Tokenizer *tokenizer)
{
    scanner = tokenizer;
}

void Parser::Parse()
{
    scanner->Next();
    tree.root = parseRelationalExpr();
}

// primary-expr ::= id | constant | string-literal | (expr)

PrimaryExprNode *Parser::parsePrimaryExpr()
{
    Token *t = scanner->Current();
    switch (t->type)
    {
        case TokenType::NUM_INT:
            scanner->Next();
            return new IntConstNode(t);
        case TokenType::NUM_FLOAT:
            scanner->Next();
            return new FloatConstNode(t);
        case TokenType::ID:
            scanner->Next();
            return new IdNode(t);
        case TokenType::STRING:
            scanner->Next();
            return new StringLiteralNode(t);
        case TokenType::LBRACKET:
            scanner->Next();
            PrimaryExprNode *e = parsePrimaryExpr(); // that's temporary function call;
            if (scanner->Current()->type != TokenType::RBRACKET)
                throw SyntaxError(t, "Missing closing bracket. ");
            scanner->Next();
            return e;
    }
    throw SyntaxError(t, "Missing operand. ");
}


std::ostream &operator<<(std::ostream &os, Parser &parser)
{
    os << parser.tree;
    return os;
}

PostfixExprNode *Parser::parsePostrixExpr()
{
    PostfixExprNode *pe = parsePrimaryExpr();
    Token *t = scanner->Current();
    bool canBeContinued = true;
    while (canBeContinued)
        switch (t->type)
        {
            case TokenType::DOUBLE_PLUS:
                t = scanner->Next();
                pe = new PostfixIncrementNode(pe);
                break;
            case TokenType::DOUBLE_MINUS:
                t = scanner->Next();
                pe = new PostfixDecrementNode(pe);
                break;
            case TokenType::DOT:
                t = scanner->Next();
                if (t->type != TokenType::ID) throw "";
                pe = new StructureOrUnionMemberAccessNode(pe, new IdNode(t));
                t = scanner->Next();
                break;
            case TokenType::ARROW:
                t = scanner->Next();
                if (t->type != TokenType::ID) throw "";
                pe = new StructureOrUnionMemberAccessByPointerNode(pe, new IdNode(t));
                t = scanner->Next();
                break;
            default:
                canBeContinued = false;
        }
    return pe;
}

PostfixExprNode *Parser::parseUnaryExpr()
{
    PostfixExprNode *ue;
    Token *t = scanner->Current();
    switch (t->type)
    {
        case TokenType::DOUBLE_PLUS:
            t = scanner->Next();
            ue = new PrefixIncrementNode(parseUnaryExpr());
            break;
        case TokenType::DOUBLE_MINUS:
            t = scanner->Next();
            ue = new PrefixDecrementNode(parseUnaryExpr());
            break;
        default:
            ue = parsePostrixExpr();
    }
    return ue;
}

PostfixExprNode *Parser::parseCastExpr()
{
    return parseUnaryExpr();
}

PostfixExprNode *Parser::parseMultiplicativeExpr()
{
    PostfixExprNode *ce = parseCastExpr();
    Token *t = scanner->Current();
    while (t->type == TokenType::ASTERIX || t->type == TokenType::FORWARD_SLASH || t->type == TokenType::REMINDER)
    {
        scanner->Next();
        auto right = parseCastExpr();
        ce = new BinOpNode(ce, right, t);
        t = scanner->Current();
    }
    return ce;
}

PostfixExprNode *Parser::parseAddictiveExpr()
{
    PostfixExprNode *me = parseMultiplicativeExpr();
    Token *t = scanner->Current();
    while (t->type == TokenType::MINUS || t->type == TokenType::PLUS)
    {
        scanner->Next();
        auto right = parseMultiplicativeExpr();
        me = new BinOpNode(me, right, t);
        t = scanner->Current();
    }
    return me;
}

PostfixExprNode *Parser::parseShiftExpr()
{
    PostfixExprNode *ae = parseAddictiveExpr();
    Token *t = scanner->Current();
    while (t->type == TokenType::BITWISE_LSHIFT || t->type == TokenType::BITWISE_RSHIFT)
    {
        scanner->Next();
        auto right = parseAddictiveExpr();
        ae = new BinOpNode(ae, right, t);
        t = scanner->Current();
    }
    return ae;
}

PostfixExprNode *Parser::parseRelationalExpr()
{
    PostfixExprNode *se = parseShiftExpr();
    Token *t = scanner->Current();
    while (t->type == TokenType::RELOP_LT || t->type == TokenType::RELOP_GT || t->type == TokenType::RELOP_LE
            || t->type == TokenType::RELOP_GE)
    {
        scanner->Next();
        auto right = parseShiftExpr();
        se = new BinOpNode(se, right, t);
        t = scanner->Current();
    }
    return se;
}
