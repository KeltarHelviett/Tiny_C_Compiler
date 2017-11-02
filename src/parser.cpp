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
    scanner->Next();
    tree.root = parseInitializerList();
}

// primary-expr ::= id | constant | string-literal | (expr)

PostfixExprNode *Parser::parsePrimaryExpr()
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
            PostfixExprNode *e = parseExpr(); // that's temporary function call;
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

PostfixExprNode *Parser::parsePostfixExpr()
{
    Token *t = scanner->Current();
    PostfixExprNode *pe = parsePrimaryExpr();
    t = scanner->Current();
    bool stillPostfixOperator = true;
    while (stillPostfixOperator)
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
            case TokenType::LSQUARE_BRACKET:
                t = scanner->Next();
                pe = new ArrayAccess(pe, parseExpr());
                if ((t = scanner->Current())->type != TokenType::RSQUARE_BRACKER) throw "";
                t = scanner->Next();
                break;
            case TokenType::LBRACKET:
                t = scanner->Next();
                pe = new FunctionCallNode(pe, parseArgumentExprList());
                if (scanner->Current()->type != TokenType::RBRACKET) throw "";
                t = scanner->Next();
                break;
            default:
                stillPostfixOperator = false;
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
        case TokenType::KEYWORD:
            if (t->keyword == Keyword::SIZEOF)
            {
                t = scanner->Next();
                if (t->type == TokenType::LBRACKET)
                {
                    scanner->Next();
                    ue = new SizeofTypeNameNode(parseTypeName());
                    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
                    scanner->Next();
                }
                else
                    ue = new SizeofExprNode(parseUnaryExpr());
            }
            else throw "";
            break;
        default:
            if (isUnaryOp(t))
            {
                scanner->Next();
                ue = new UnaryOpNode(t, parseCastExpr());
                t = scanner->Current();
                break;
            }
            ue = parsePostfixExpr();
    }
    return ue;
}

PostfixExprNode *Parser::parseCastExpr()
{
    if (scanner->Current()->type == TokenType::LBRACKET &&
            (isTypeQualifier(scanner->Peek()) || isTypeSpecifier(scanner->Peek())))
    {
        scanner->Next();
        TypeNameNode *typName = parseTypeName();
        if (scanner->Current()->type != TokenType::RBRACKET) throw "";
        scanner->Next();
        return new TypeCastNode(typName, parseCastExpr());
    }
    return parseUnaryExpr();
}

PostfixExprNode *Parser::parseMultiplicativeExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::ASTERIX, TokenType::FORWARD_SLASH, TokenType::REMINDER};
    return parseGeneral(this, &Parser::parseCastExpr, types);
}

PostfixExprNode *Parser::parseAddictiveExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::PLUS, TokenType::MINUS};
    return parseGeneral(this, &Parser::parseMultiplicativeExpr, types);
}

PostfixExprNode *Parser::parseShiftExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_LSHIFT, TokenType::BITWISE_RSHIFT};
    return parseGeneral(this, &Parser::parseAddictiveExpr, types);
}

PostfixExprNode *Parser::parseRelationalExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::RELOP_GT, TokenType::RELOP_LT,
                                                  TokenType::RELOP_GE, TokenType::RELOP_LE};
    return parseGeneral(this, &Parser::parseShiftExpr, types);
}

PostfixExprNode *Parser::parseEqualityExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::RELOP_EQ, TokenType::RELOP_NE};
    return parseGeneral(this, &Parser::parseRelationalExpr, types);
}

PostfixExprNode *Parser::parseAndExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_AND};
    return parseGeneral(this, &Parser::parseEqualityExpr, types);
}

PostfixExprNode *Parser::parseExclusiveOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_XOR};
    return parseGeneral(this, &Parser::parseAndExpr, types);
}

PostfixExprNode *Parser::parseInclusiveOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_OR};
    return parseGeneral(this, &Parser::parseExclusiveOrExpr, types);
}

PostfixExprNode *Parser::parseLogicalAndExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::LOGIC_AND};
    return parseGeneral(this, &Parser::parseInclusiveOrExpr, types);
}

PostfixExprNode *Parser::parseLogicalOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::LOGIC_OR};
    return parseGeneral(this, &Parser::parseLogicalAndExpr, types);
}

PostfixExprNode *Parser::parseGeneral(Parser *self, PostfixExprNode *(Parser::*f)(),
                                      std::unordered_set<TokenType> types)
{
    auto *e = (PostfixExprNode *)(self->*f)();
    Token *t = self->scanner->Current();
    while (types.find(t->type) != types.end())
    {
        self->scanner->Next();
        auto right = (PostfixExprNode *) ((*self).*f)();
        e = new BinOpNode(e, right, t);
        t = self->scanner->Current();
    }
    return e;
}

PostfixExprNode *Parser::parseConditionalExpr()
{
    PostfixExprNode *loe =  parseLogicalOrExpr();
    Token *t = scanner->Current();
    if (t->type == TokenType::QUESTION_MARK)
    {
        t = scanner->Next();
        PostfixExprNode *then = parseExpr();
        t = scanner->Current();
        if (t->type != TokenType::COLON)
            throw "";
        t = scanner->Next();
        PostfixExprNode *lse = parseConditionalExpr();
        return new TernaryOperatorNode(loe, then, lse);
    }
    return loe;
}

PostfixExprNode *Parser::parseAssignmentExpr()
{
    PostfixExprNode *ce = parseConditionalExpr();
    Token *t = scanner->Current();
    if (isAssignmentOp(t))
    {
        scanner->Next();
        return new AssignmentNode(ce, parseAssignmentExpr(), t);

    }
    return ce;
}

PostfixExprNode *Parser::parseExpr()
{
    PostfixExprNode *ae = parseAssignmentExpr();
    while (scanner->Current()->type == TokenType::COMMA)
    {
        scanner->Next();
        ae = new CommaSeparatedExprs(ae, parseExpr());
    }
    return ae;
}

TypeNameNode *Parser::parseTypeName()
{
    auto tnn = parseSpecifierQualifierList();
    auto abstractDeclarator = parseDeclarator(DeclaratorType::ABSTRACT);
    if (tnn->Size() == 0) throw "";
    return new TypeNameNode(tnn, abstractDeclarator);
}

bool Parser::isTypeSpecifier(Token *token)
{
    return token->type == TokenType::KEYWORD ? TypeSpecifiers.find(token->keyword) != TypeSpecifiers.end() : false;
}

bool Parser::isUnaryOp(Token *token)
{
    return UnaryOps.find(token->type) != UnaryOps.end();
}

bool Parser::isAssignmentOp(Token *token)
{
    return  AssignmentOps.find(token->type) != AssignmentOps.end();
}

bool Parser::isTypeQualifier(Token *token)
{
    return token->type == TokenType::KEYWORD ? TypeQualifiers.find(token->keyword) != TypeQualifiers.end() : false;
}

PostfixExprNode *Parser::parseConstantExpr()
{
    return parseConditionalExpr();
}

TypeQualifierListNode *Parser::parseTypeQualifierList()
{
    auto tql = new TypeQualifierListNode();
    Token *t = scanner->Current();
    while (isTypeQualifier(t))
    {

        tql->Add(new TypeQualifierNode(t));
        t = scanner->Next();
    }
    return tql;
}

PointerNode *Parser::parsePointer()
{
    if (scanner->Current()->type != TokenType::ASTERIX) return nullptr;
    scanner->Next();
    return new PointerNode(parseTypeQualifierList(), parsePointer());
}

StatementNode *Parser::parseStatement()
{
    if (scanner->Current()->type == TokenType::KEYWORD)
        switch (scanner->Current()->keyword)
        {
            case Keyword::IF:
                return parseSelectionStatement();
            case Keyword::GOTO: case Keyword::CONTINUE: case Keyword::BREAK:
            case Keyword::RETURN:
                return parseJumpStatement();
            case Keyword::FOR: case Keyword::DO: case Keyword::WHILE:
                return parseIterationStatement();
        }
    if (scanner->Current()->type == TokenType::ID && scanner->Peek()->type == TokenType::COLON)
        return parseLabelStatement();
    if (scanner->Current()->type == TokenType::LCURLY_BRACKET)
        return parseCompoundStatement();
    return reinterpret_cast<StatementNode *>(parseExprStatement());
}

ExprStatmentNode *Parser::parseExprStatement()
{
    if (scanner->Current()->type == TokenType::SEMICOLON)
    {
        scanner->Next();
        return new ExprStatmentNode(nullptr);
    }
    auto et = new ExprStatmentNode(parseExpr());
    if (scanner->Current()->type != TokenType::SEMICOLON) throw "";
    scanner->Next();
    return et;
}

SelectionStatementNode *Parser::parseSelectionStatement()
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::IF) throw "";
    scanner->Next();
    if (scanner->Current()->type != TokenType::LBRACKET) throw "";
    scanner->Next();
    auto expr = parseExpr();
    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
    scanner->Next();
    auto then = parseStatement();
    if (scanner->Current()->type == TokenType::KEYWORD && scanner->Current()->keyword == Keyword::ELSE)
    {
        scanner->Next();
        return new IfElseStatementNode(expr, then, parseStatement());
    }
    return new IfStatementNode(expr, then);
}

JumpStatementNode *Parser::parseJumpStatement()
{
    JumpStatementNode *js = nullptr;
    if (scanner->Current()->type == TokenType::KEYWORD)
        switch (scanner->Current()->keyword)
        {
            case Keyword::GOTO:
                if (scanner->Next()->type != TokenType::ID) throw "";
                js = new GotoStatementNode(new IdNode(scanner->Current()));
                scanner->Next();
                break;
            case Keyword::CONTINUE:
                scanner->Next()->type;
                js = new ContinueStatementNode();
                break;
            case Keyword::BREAK:
                scanner->Next()->type;
                js = new BreakStatementNode();
                break;
            case Keyword::RETURN:
                js = scanner->Next()->type == TokenType::SEMICOLON ? new ReturnStatementNode(nullptr) :
                     new ReturnStatementNode(parseExpr());
                break;
            default:
                throw "";
        }
    if (scanner->Current()->type != TokenType::SEMICOLON) throw "";
    scanner->Next();
    return js;
}

IterationStatementNode *Parser::parseIterationStatement()
{
    if (scanner->Current()->type != TokenType::KEYWORD) throw "";
    switch (scanner->Current()->keyword)
    {
        case Keyword::FOR:
            return parseForStatement();
        case Keyword::DO:
            return parseDoWhileStatement();
        case Keyword::WHILE:
            return parseWhileStatement();

    }
}

ForStatementNode *Parser::parseForStatement()
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::FOR) throw "";
    if (scanner->Next()->type != TokenType::LBRACKET) throw "";
    scanner->Next();
    auto init = parseExprStatement(), condition = parseExprStatement();
    if (scanner->Current()->type == TokenType::RBRACKET)
    {
        scanner->Next();
        return new ForStatementNode(init, condition, nullptr, parseStatement());
    }
    auto iteration = parseExpr();
    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
    scanner->Next();
    return new ForStatementNode(init, condition, iteration, parseStatement());
}

WhileStatementNode *Parser::parseWhileStatement()
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::WHILE) throw "";
    if (scanner->Next()->type != TokenType::LBRACKET) throw "";
    scanner->Next();
    auto condition = parseExpr();
    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
    scanner->Next();
    return new WhileStatementNode(condition, parseStatement());
}

DoWhileStatementNode *Parser::parseDoWhileStatement()
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::DO) throw "";
    scanner->Next();
    auto body = parseStatement();
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::WHILE) throw "";
    if (scanner->Next()->type != TokenType::LBRACKET) throw "";
    scanner->Next();
    auto condition = parseExpr();
    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
    if (scanner->Next()->type != TokenType::SEMICOLON) throw "";
    scanner->Next();
    return new DoWhileStatementNode(condition, body);
}

DeclaratorNode *Parser::parseDeclarator(DeclaratorType type)
{
    PointerNode *pointer = nullptr;
    if (scanner->Current()->type == TokenType::ASTERIX) pointer = parsePointer();
    return new DeclaratorNode(pointer, parseDirectDeclarator(type));
}

DirectDeclaratorNode *Parser::parseDirectDeclarator(DeclaratorType type)
{
    if (scanner->Current()->type == TokenType::LBRACKET)
    {
        scanner->Next();
        auto declarator = parseDeclarator(type);
        if (scanner->Current()->type != TokenType::RBRACKET) throw "";
        scanner->Next();
        return (DirectDeclaratorNode *)declarator;
    }
    DirectDeclaratorNode *directDeclarator = nullptr;
    if (type == DeclaratorType::NORMAL && scanner->Current()->type != TokenType::ID) throw "";
    if (type != DeclaratorType::ABSTRACT && scanner->Current()->type == TokenType::ID)
    {
        directDeclarator = (DirectDeclaratorNode *)new IdNode(scanner->Current());
        scanner->Next();
    }
    while (scanner->Current()->type == TokenType::LSQUARE_BRACKET || scanner->Current()->type == TokenType::LBRACKET)
    {
        if (scanner->Current()->type == TokenType::LSQUARE_BRACKET)
            directDeclarator = parseArrayDeclarator(directDeclarator);
        else
            directDeclarator = parseFunctionDeclarator(directDeclarator);
    }
    return directDeclarator;
}

ArrayDeclaratorNode *Parser::parseArrayDeclarator(DirectDeclaratorNode *directDeclarator)
{
    if (scanner->Current()->type == TokenType::RSQUARE_BRACKER)
        return new ArrayDeclaratorNode(directDeclarator, nullptr);
    scanner->Next();
    auto ce = parseConstantExpr();
    if (scanner->Current()->type != TokenType::RSQUARE_BRACKER) throw "";
    scanner->Next();
    return new ArrayDeclaratorNode(directDeclarator, (ConditionalExprNode *)(ce));
}

ArgumentExprListNode *Parser::parseArgumentExprList()
{
    auto ael = new ArgumentExprListNode();
    while (scanner->Current()->type != TokenType::RBRACKET)
    {
        ael->Add((AssignmentExprNode *)parseAssignmentExpr());
        if (scanner->Current()->type != TokenType::RBRACKET)
        {
            if (scanner->Current()->type != TokenType::COMMA) throw "";
            scanner->Next();
        }
    }
    return ael;
}

bool Parser::isStorageClassSpecifier(Token *token)
{
    return token->type == TokenType::KEYWORD ?
           StorageClassSpecifiers.find(token->keyword) != StorageClassSpecifiers.end() : false;
}

bool Parser::isFunctionSpecifier(Token *token)
{
    return token->type == TokenType::KEYWORD && token->keyword == Keyword::INLINE;
}

ParameterDeclarationNode *Parser::parseParameterDeclaration()
{
    return new ParameterDeclarationNode(parseDeclarationSpecifiers(), parseDeclarator(DeclaratorType::ABSTRACT_OR_NORMAL));
}

DeclarationSpecifiersNode *Parser::parseDeclarationSpecifiers()
{
    auto ds = new DeclarationSpecifiersNode();
    while(isDeclarationSpecifier(scanner->Current()))
    {
        if (scanner->Current()->keyword == Keyword::STRUCT)
            ds->Add((DeclarationSpecifierNode *)parseStructSpecifier());
        else if (scanner->Current()->keyword == Keyword::ENUM)
            ds->Add((DeclarationSpecifierNode *)parseEnumSpecifier());
        else
        {
            ds->Add(new SimpleSpecifier(scanner->Current()));
            scanner->Next();
        }
    }
    if (!ds->Size()) throw "";
    return ds;
}

ParameterList *Parser::parseParameterList()
{
    auto pl = new ParameterList();
    do
    {
        pl->Add(parseParameterDeclaration());
    } while(scanner->Current()->type == TokenType::COMMA && scanner->Next());
    if (!pl->Size()) throw "";
    return pl;
}

ParameterTypeList *Parser::parseParameterTypeList()
{
    return parseParameterList();
}

bool Parser::isDeclarationSpecifier(Token *token)
{
    return isTypeSpecifier(token) || isStorageClassSpecifier(token) || isTypeQualifier(token)
           || isFunctionSpecifier(token);
}

FunctionDeclaratorNode *Parser::parseFunctionDeclarator(DirectDeclaratorNode *directDeclarator)
{
    scanner->Next();
    auto ptl = parseParameterTypeList();
    if (scanner->Current()->type != TokenType::RBRACKET) throw "";
    scanner->Next();
    return new FunctionDeclaratorNode(directDeclarator, ptl);
}

DeclarationNode * Parser::parseDeclaration()
{
    auto ds = parseDeclarationSpecifiers();
    if (scanner->Current()->type == TokenType::SEMICOLON)
    {
        scanner->Next();
        return new DeclarationNode(ds, nullptr);
    }
    auto idl = parseInitDeclaratorList();
    if (scanner->Current()->type != TokenType::SEMICOLON) throw "";
    scanner->Next();
    return new DeclarationNode(ds, idl);
}

InitDeclaratorListNode *Parser::parseInitDeclaratorList()
{
    auto idl = new InitDeclaratorListNode();
    do
    {
        idl->Add(parseInitDeclarator());
    } while (scanner->Current()->type == TokenType::COMMA && scanner->Next());
    return idl;
}

InitDeclaratorNode *Parser::parseInitDeclarator()
{
    auto dcltr = parseDeclarator(DeclaratorType::NORMAL);
    InitializerNode *initializer = nullptr;
    if (scanner->Current()->type == TokenType::ASSIGNMENT)
    {
        scanner->Next();
        initializer = parseInitializer();
    }
    return new InitDeclaratorNode(dcltr, initializer);
}

InitializerNode *Parser::parseInitializer()
{
    if (scanner->Current()->type == TokenType::LCURLY_BRACKET)
        return (InitializerNode *)parseInitializerList();
    return (InitializerNode *)parseAssignmentExpr();
}

LabelStatementNode *Parser::parseLabelStatement()
{
    if (scanner->Current()->type != TokenType::ID) throw "";
    auto id = new IdNode(scanner->Current());
    scanner->Next();
    if (scanner->Current()->type != TokenType::COLON) throw "";
    scanner->Next();
    return new LabelStatementNode(id, parseStatement());
}

CompoundStatement *Parser::parseCompoundStatement()
{
    if (scanner->Current()->type != TokenType::LCURLY_BRACKET) throw "";
    if (scanner->Next()->type == TokenType::RCURLY_BRACKET) return new CompoundStatement(nullptr);
    auto blockItemList = parseBlockItemList();
    if (scanner->Current()->type != TokenType::RCURLY_BRACKET) throw "";
    return new CompoundStatement(blockItemList);
}

BlockItemListNode *Parser::parseBlockItemList()
{
    auto blockItemList = new BlockItemListNode();
    while (scanner->Current()->type != TokenType::RCURLY_BRACKET)
    {
        blockItemList->Add(parseBlockItem());
    }
    return blockItemList;
}

BlockItemNode *Parser::parseBlockItem()
{
    return (isDeclarationSpecifier(scanner->Current()) ? (BlockItemNode *)parseDeclaration() : (BlockItemNode *)parseStatement());
}

EnumSpecifierNode *Parser::parseEnumSpecifier()
{
    IdNode *id = nullptr;
    EnumeratorList *list = nullptr;
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::ENUM) throw "";
    if (scanner->Next()->type == TokenType::ID)
    {
        id = new IdNode(scanner->Current());
        scanner->Next();
    }
    if (!id && scanner->Current()->type != TokenType::LCURLY_BRACKET) throw "";
    if (scanner->Current()->type == TokenType::LCURLY_BRACKET && scanner->Next())
    {
        list = parseEnumeratorList();
        if (!list->Size()) throw "";
        if (scanner->Current()->type != TokenType::RCURLY_BRACKET) throw "";
        scanner->Next();
    }
    return new EnumSpecifierNode(id, list);
}

EnumeratorList *Parser::parseEnumeratorList()
{
    auto list = new EnumeratorList();
    do
    {
        if (scanner->Current()->type == TokenType::RCURLY_BRACKET) break;
        list->Add(parseEnumerator());
    } while(scanner->Current()->type == TokenType::COMMA && scanner->Next());
    return list;
}

EnumeratorNode *Parser::parseEnumerator()
{
    if (scanner->Current()->type != TokenType::ID) throw "";
    auto id = new IdNode(scanner->Current());
    scanner->Next();
    if (scanner->Current()->type == TokenType::ASSIGNMENT && scanner->Next())
        return new EnumeratorNode(id, (ConstantExprNode *)parseConstantExpr());
    return new EnumeratorNode(id, nullptr);
}

bool Parser::isSimpleSpecifier(Token *token)
{
    return isTypeQualifier(token) || isTypeSpecifier(token) ||
            isStorageClassSpecifier(token) || isFunctionSpecifier(token);
}

StructSpecifierNode *Parser::parseStructSpecifier()
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != Keyword::STRUCT) throw "";
    scanner->Next();
    IdNode *id = scanner->Current()->type == TokenType::ID ? new IdNode(scanner->Current()) : nullptr;
    if (!id)
    {
        if (scanner->Current()->type != TokenType::LCURLY_BRACKET) throw "";
        scanner->Next();
        return new StructSpecifierNode(id, parseStructDeclarationList());
    }
    scanner->Next();
    if (scanner->Current()->type == TokenType::LCURLY_BRACKET && scanner->Next())
        return new StructSpecifierNode(id, parseStructDeclarationList());
    return new StructSpecifierNode(id, nullptr);
}

StructDeclarationListNode *Parser::parseStructDeclarationList()
{
    auto sdl = new StructDeclarationListNode();
    do
    {
        sdl->Add(parseStructDeclaration());
    } while (scanner->Current()->type != TokenType::RCURLY_BRACKET);
    scanner->Next();
    return sdl;
}
//struct-declaration ::= specifier-qualifier-list struct-declarator-list ;
StructDeclarationNode *Parser::parseStructDeclaration()
{
    auto structDecl = new StructDeclarationNode(parseSpecifierQualifierList(), parseStructDeclaratorList());
    if (scanner->Current()->type != TokenType::SEMICOLON) throw "";
    scanner->Next();
    return structDecl;
}

StructDeclaratorNode *Parser::parseStructDeclarator()
{
    if (scanner->Current()->type == TokenType::COLON && scanner->Next())
        return new StructDeclaratorNode(nullptr, (ConstantExprNode *)parseConstantExpr());
    auto declarator = parseDeclarator(DeclaratorType::NORMAL);
    if (scanner->Current()->type == TokenType::COLON && scanner->Next())
        return new StructDeclaratorNode(declarator, (ConstantExprNode *)parseConstantExpr());
    return new StructDeclaratorNode(declarator, nullptr);
}


StructDeclaratorListNode *Parser::parseStructDeclaratorList()
{
    auto sdl = new StructDeclaratorListNode();
    do
    {
        sdl->Add(parseStructDeclarator());
    } while (scanner->Current()->type == TokenType::COMMA && scanner->Next());
    return sdl;
}

SpecifierQualifierListNode *Parser::parseSpecifierQualifierList()
{
    auto tnn = new SpecifierQualifierListNode();
    Token *t = scanner->Current();
    bool spec;
    while ((spec = isTypeSpecifier(t)) || isTypeQualifier(t))
    {

        tnn->Add(spec ? (TypeSpecifierQualifierNode *)new TypeSpecifierNode(t) :
                 (TypeSpecifierQualifierNode *)new TypeQualifierNode(t)) ;
        t = scanner->Next();
    }
    return tnn;
}
//initializer-list ::= `designation initializer | initializer-list , `designation initializer
InitializerListNode *Parser::parseInitializerList()
{
    if (scanner->Current()->type != TokenType::LCURLY_BRACKET) throw "";
    scanner->Next();
    auto il = new InitializerListNode();
    do
    {
        if (scanner->Current()->type == TokenType::RCURLY_BRACKET) break;
        il->Add(parseDesignatedInitializer());
    } while (scanner->Current()->type == TokenType::COMMA && scanner->Next());
    return il;
}

DesignationNode *Parser::parseDesignation()
{
    auto designatorList = parseDesignatorList();
    if (scanner->Current()->type != TokenType::ASSIGNMENT) throw "";
    scanner->Next();
    return new DesignationNode(designatorList);
}

DesignatorListNode *Parser::parseDesignatorList()
{
    auto dl = new DesignatorListNode();
    do
    {
        dl->Add(parseDesignator());
    } while(scanner->Current()->type == TokenType::LSQUARE_BRACKET || scanner->Current()->type == TokenType::DOT);
    return dl;
}

DesignatorNode *Parser::parseDesignator()
{
    if (scanner->Current()->type == TokenType::LSQUARE_BRACKET)
    {
        scanner->Next();
        auto constExpr = parseConstantExpr();
        if (scanner->Current()->type != TokenType::RSQUARE_BRACKER) throw "";
        scanner->Next();
        return new ArrayDesignator((ConstantExprNode *)constExpr);
    }
    if (scanner->Current()->type != TokenType::DOT) throw "";
    scanner->Next();
    if (scanner->Current()->type != TokenType::ID) throw "";
    auto smd = new StructMemberDesignator(new IdNode(scanner->Current()));
    scanner->Next();
    return smd;
}

DesignatedInitializerNode *Parser::parseDesignatedInitializer()
{
    if (scanner->Current()->type == TokenType::DOT || scanner->Current()->type == TokenType::LSQUARE_BRACKET)
        return new DesignatedInitializerNode(parseDesignation(), parseInitializer());
    return new DesignatedInitializerNode(nullptr, parseInitializer());
}
