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
    tree.root = parseTranslationUnit();
}

std::ostream &operator<<(std::ostream &os, Parser &parser)
{
    os << parser.tree;
    return os;
}

// primary-expr ::= id | constant | string-literal | (expr)

ExprNode *Parser::parsePrimaryExpr()
{
    auto t = scanner->Current();
    scanner->Next();
    switch (t->type)
    {
        case TokenType::NUM_INT:
            return new IntConstNode(t);
        case TokenType::NUM_FLOAT:
            return new FloatConstNode(t);
        case TokenType::ID:
            return sematicAnalyzer.BuildIdNode(t);
        case TokenType::STRING:
            return new StringLiteralNode(t);
        case TokenType::LBRACKET:
            ExprNode *e = parseExpr();
            requireNext(TokenType::RBRACKET);
            return e;
    }
    throw InvalidExpression(scanner->Current());
}

//postfix-expr ::= primary-expr | postifx-expr [expr] | postfix-expr (`argument-expr-list)
//| postfix-expr . id | postfix-expr -> id | postfix-expr ++ | postfix-expr --
//| (type-id) {initializer-list} | (type-id) {initializer-list, }

ExprNode *Parser::parsePostfixExpr()
{
    auto t = scanner->Current();
    ExprNode *pe = parsePrimaryExpr();
    t = scanner->Current();
    bool stillPostfixOperator = true;

    while (stillPostfixOperator)
    {
        auto op = t;
        switch (t->type)
        {
            case TokenType::DOUBLE_PLUS:
                pe = sematicAnalyzer.BuildPostfixIncrementNode(pe, op);
                t = scanner->Next();
                break;
            case TokenType::DOUBLE_MINUS:
                pe = sematicAnalyzer.BuildPostfixDecrementNode(pe, op);
                t = scanner->Next();
                break;
            case TokenType::DOT:
                t = scanner->Next();
                require(TokenType::ID);
                pe = sematicAnalyzer.BuildStructureOrUnionMemberAccessNode(pe, new IdNode(t), op);
                t = scanner->Next();
                break;
            case TokenType::ARROW:
                t = scanner->Next();
                require(TokenType::ID);
                pe = sematicAnalyzer.BuildStructureOrUnionMemberAccessByPointerNode(pe, new IdNode(t), op);
                t = scanner->Next();
                break;
            case TokenType::LSQUARE_BRACKET:
                t = scanner->Next();
                pe = sematicAnalyzer.BuildArrayAccessNode(pe, parseExpr());
                require(TokenType::RSQUARE_BRACKET);
                t = scanner->Next();
                break;
            case TokenType::LBRACKET:
                t = scanner->Next();
                pe = sematicAnalyzer.BuildFunctionCallNode(pe, parseArgumentExprList());
                require(TokenType::RBRACKET);
                t = scanner->Next();
                break;
            default:
                stillPostfixOperator = false;
        }
    }
    return pe;
}

//unary-expr ::= postfix-expr | ++ unary-expr | -- unary-expr | unary-op cast-expr
//| sizeof unary-expr | sizeof (type-id)

ExprNode *Parser::parseUnaryExpr()
{
    ExprNode *ue;
    auto t = scanner->Current(), op = t;
    switch (t->type)
    {
        case TokenType::DOUBLE_PLUS:
            t = scanner->Next();
            ue = sematicAnalyzer.BuildPrefixIncrementNode(parseUnaryExpr(), op);
            break;
        case TokenType::DOUBLE_MINUS:
            t = scanner->Next();
            ue = sematicAnalyzer.BuildPrefixDecrementNode(parseUnaryExpr(), op);
            break;
        case TokenType::KEYWORD:
            if (t->keyword == Keyword::SIZEOF)
            {
                t = scanner->Next();
                if (t->type == TokenType::LBRACKET)
                {
                    scanner->Next();
                    ue = new SizeofTypeNameNode(parseTypeName());
                    require(TokenType::RBRACKET);
                    scanner->Next();
                }
                else
                    ue = new SizeofExprNode(parseUnaryExpr());
            }
            else if (t->keyword == Keyword::PRINTF)
            {
                ArgumentExprListNode *arguments = nullptr;
                scanner->Next();
                requireNext(TokenType::LBRACKET);
                t = scanner->Current();
                requireNext(TokenType::STRING);
                if (maybeNext(TokenType::COMMA))
                    arguments = parseArgumentExprList();
                requireNext(TokenType::RBRACKET);
                ue = sematicAnalyzer.BuildPrintfNode(new StringLiteralNode(t), arguments);
            }
            break;
        default:
            if (isUnaryOp(t))
            {
                scanner->Next();
                ue = sematicAnalyzer.BuildUnaryOpNode(t, parseCastExpr());
                t = scanner->Current();
                break;
            }
            ue = parsePostfixExpr();
    }
    return ue;
}

//cast-expr ::= unary-expr | (type-id) cast-expr

ExprNode *Parser::parseCastExpr()
{
    if (scanner->Current()->type == TokenType::LBRACKET &&
            (isTypeQualifier(scanner->Peek()) || isTypeSpecifier(scanner->Peek())))
    {
        scanner->Next();
        auto typName = parseTypeName();
        requireNext(TokenType::RBRACKET);
        return sematicAnalyzer.BuildTypeCastNode(typName, parseCastExpr());
    }
    return parseUnaryExpr();
}

//multiplicative-expr ::= cast-expr | multiplicative-expr * cast-expr
//                        | multiplicative-expr / cast-expr
//                        | multiplicative-expr % cast-expr

ExprNode *Parser::parseMultiplicativeExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::ASTERIX, TokenType::FORWARD_SLASH, TokenType::REMINDER};
    return parseBinoOps(this, &Parser::parseCastExpr, types);
}

//addictive-expr ::= multiplicative-expr
//                   | addictive-expr + multiplicative-expr
//                   | addictive-expr - multiplicative-expr

ExprNode *Parser::parseAddictiveExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::PLUS, TokenType::MINUS};
    return parseBinoOps(this, &Parser::parseMultiplicativeExpr, types);
}

//shift-expr ::= addictive-expr
//               | shift-expr << addictive-expr
//               | shift-expr >> addictive-expr

ExprNode *Parser::parseShiftExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_LSHIFT, TokenType::BITWISE_RSHIFT};
    return parseBinoOps(this, &Parser::parseAddictiveExpr, types);
}

//rel-expr ::= shift-expr | rel-expr < shift-expr | rel-expr > shift-expr
//             | rel-expr <= shift expr | rel-expr >= shift-expr

ExprNode *Parser::parseRelationalExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::RELOP_GT, TokenType::RELOP_LT,
                                                  TokenType::RELOP_GE, TokenType::RELOP_LE};
    return parseBinoOps(this, &Parser::parseShiftExpr, types);
}

//eq-expr ::= rel-expr | eq-expr == rel-expr | eq-expr != rel-expr

ExprNode *Parser::parseEqualityExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::RELOP_EQ, TokenType::RELOP_NE};
    return parseBinoOps(this, &Parser::parseRelationalExpr, types);
}

//AND-expr ::= eq-expr | AND-expr & eq-expr

ExprNode *Parser::parseAndExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_AND};
    return parseBinoOps(this, &Parser::parseEqualityExpr, types);
}

//exclusive-OR-expr ::= AND-expr | exclusive-OR-expr ^ AND-expr

ExprNode *Parser::parseExclusiveOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_XOR};
    return parseBinoOps(this, &Parser::parseAndExpr, types);
}

//inclusive-OR-expr ::= exclusive-OR-expr | inclusive-OR-expr '|' exclusive-OR-expr

ExprNode *Parser::parseInclusiveOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::BITWISE_OR};
    return parseBinoOps(this, &Parser::parseExclusiveOrExpr, types);
}

//logical-AND-expr ::= inclusive-OR-expr | logical-AND-expr && inclusive-OR-expr

ExprNode *Parser::parseLogicalAndExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::LOGIC_AND};
    return parseBinoOps(this, &Parser::parseInclusiveOrExpr, types);
}

//logical-OR-expr ::= logical-AND-expr | logical-OR-expr || logical-AND-expr

ExprNode *Parser::parseLogicalOrExpr()
{
    static std::unordered_set<TokenType> types = {TokenType::LOGIC_OR};
    return parseBinoOps(this, &Parser::parseLogicalAndExpr, types);
}

ExprNode *Parser::parseBinoOps(Parser *self, ExprNode *(Parser::*f)(),
                               std::unordered_set<TokenType> types)
{
    auto *e = (ExprNode *)(self->*f)();
    auto t = self->scanner->Current();
    while (types.find(t->type) != types.end())
    {
        self->scanner->Next();
        auto right = (ExprNode *) ((*self).*f)();
        e = sematicAnalyzer.BuildBinOpNode(e, right, t);
        t = self->scanner->Current();
    }
    return e;
}

//conditional-expr ::= logical-OR-expr | logical-OR-expr ? expr : conditional-expr

ExprNode *Parser::parseConditionalExpr()
{
    ExprNode *loe =  parseLogicalOrExpr();
    auto t = scanner->Current(), q = t;
    if (t->type == TokenType::QUESTION_MARK)
    {
        t = scanner->Next();
        ExprNode *then = parseExpr();
        auto c = t = scanner->Current();
        require(TokenType::COLON);
        t = scanner->Next();
        ExprNode *lse = parseConditionalExpr();
        return sematicAnalyzer.BuildTernaryOperatorNode(loe, then, lse, q, c);
    }
    return loe;
}

//assignment-expr ::= conditional-expr | unary-expr assignment-op assignment-expr

ExprNode *Parser::parseAssignmentExpr()
{
    ExprNode *ce = parseConditionalExpr();
    auto t = scanner->Current();
    if (isAssignmentOp(t))
    {
        scanner->Next();
        return sematicAnalyzer.BuildAssignmentNode(ce, parseAssignmentExpr(), t);
    }
    return ce;
}

//expr ::= assignment-expr | expr , assignment-expr

ExprNode *Parser::parseExpr()
{
    ExprNode *ae = parseAssignmentExpr();
    if (maybe(TokenType::COMMA))
    {
        auto list = new CommaSeparatedExprs();
        list->Add(ae);
        while (maybeNext(TokenType::COMMA))
            list->Add(parseExpr());
        return list;
    }
    return ae;
}

//type-id ::= specifier-qualifier-list `abstract-declarator

SymType *Parser::parseTypeName()
{
    auto tnn = parseSpecifierQualifierList();
    if (tnn->Size() == 0) throw NoDeclarationSpecifiers(scanner->Current());
    auto abstractDeclarator = new DeclaratorNode();
    abstractDeclarator->SetType(TypeBuilder::Build(tnn));
    parseDeclarator(DeclaratorKind::ABSTRACT, abstractDeclarator);
    return abstractDeclarator->GetType();
}

//type-specifier ::= void | char | short | int | long | float | double | singed
//                   | unsigned | struct-specifier | enum-specifier |
//                   | typedef-id

bool Parser::isTypeSpecifier(std::shared_ptr<Token> token)
{
    return (token->type == TokenType::KEYWORD ? TypeSpecifiers.find(token->keyword) != TypeSpecifiers.end() : false)
            || isTypedefIdentifier(token);
}

//unary-op  ::= & | * | + | - | ~ | !

bool Parser::isUnaryOp(std::shared_ptr<Token> token)
{
    return UnaryOps.find(token->type) != UnaryOps.end();
}

//assignment-op ::= = | *= | /= | %= | += | -= | <<= | >>= | &= | ^= | |=

bool Parser::isAssignmentOp(std::shared_ptr<Token> token)
{
    return  AssignmentOps.find(token->type) != AssignmentOps.end();
}

//type-qualifier ::= const

bool Parser::isTypeQualifier(std::shared_ptr<Token> token)
{
    return token->type == TokenType::KEYWORD ? TypeQualifiers.find(token->keyword) != TypeQualifiers.end() : false;
}

//constant-expr ::= conditional-expr

ExprNode *Parser::parseConstantExpr()
{
    return parseConditionalExpr();
}

//type-qualifier-list ::= type-qualifier | type-qualifier-list type-qualifier

DeclarationSpecifiersNode *Parser::parseTypeQualifierList()
{
    auto tql = new DeclarationSpecifiersNode();
    auto t = scanner->Current();
    while (isTypeQualifier(t))
    {
        tql->Add(new TypeQualifierNode(t));
        t = scanner->Next();
    }
    return tql;
}

//statement ::= labeled-statement | compound-statement
//              | expression-statement | selection-statement
//              | iteration-statement | jump-statement

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

//expression-statement ::= `expr ;

ExprStatmentNode *Parser::parseExprStatement()
{
    if (maybeNext(TokenType::SEMICOLON))
        return new ExprStatmentNode(nullptr);
    auto et = new ExprStatmentNode(parseExpr());
    requireNext(TokenType::SEMICOLON);
    return et;
}

//selection-statement ::= if (expr) statement
//| if (expr) statement else statement

SelectionStatementNode *Parser::parseSelectionStatement()
{
    requireKeywordNext(Keyword::IF);
    requireNext(TokenType::LBRACKET);
    auto condition = parseExpr();
    sematicAnalyzer.CheckSelectionOrIterationStatementCondition(condition);
    requireNext(TokenType::RBRACKET);
    auto body = parseStatement();
    if (maybeKeywordNext(Keyword::ELSE))
        return new IfElseStatementNode(condition, body, parseStatement());
    return new IfStatementNode(condition, body);
}

//jump-statement ::= goto id ;
//| continue ;
//| break ;
//| return `expr ;

JumpStatementNode *Parser::parseJumpStatement()
{
    JumpStatementNode *js = nullptr;
    auto t = scanner->Current();
    scanner->Next();
    if (t->type == TokenType::KEYWORD)
        switch (t->keyword)
        {
            case Keyword::GOTO:
                require(TokenType::ID);
                js = new GotoStatementNode(new IdNode(scanner->Current()));
                scanner->Next();
                break;
            case Keyword::CONTINUE:
                js = sematicAnalyzer.BuildContinueStatementNode(t);
                break;
            case Keyword::BREAK:
                js = sematicAnalyzer.BuildBreakStatementNode(t);
                break;
            case Keyword::RETURN:
                js = scanner->Current()->type == TokenType::SEMICOLON ?
                     sematicAnalyzer.BuildReturnStatementNode(t, nullptr) :
                     sematicAnalyzer.BuildReturnStatementNode(t, parseExpr());
                break;
        }
    requireNext(TokenType::SEMICOLON);
    return js;
}

//iteration-statement ::= while (expr) statement
//| do statement while (expr) ;
//| for (`expr ; `expr ; `expr) statement

IterationStatementNode *Parser::parseIterationStatement()
{
    require(TokenType::KEYWORD);
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

//for (`expr ; `expr ; `expr) statement

ForStatementNode *Parser::parseForStatement()
{
    requireKeywordNext(Keyword::FOR);
    ExprNode *init = nullptr, *condition = nullptr;
    requireNext(TokenType::LBRACKET);
    ForStatementNode *res = nullptr;
    if (!maybe(TokenType::SEMICOLON)) init = parseExpr();
    requireNext(TokenType::SEMICOLON);
    if (!maybe(TokenType::SEMICOLON)) condition = parseExpr();
    requireNext(TokenType::SEMICOLON);
    if (maybeNext(TokenType::RBRACKET))
    {
        res = new ForStatementNode(init, condition, nullptr, nullptr);
        sematicAnalyzer.ProcessLoop(res);
        res->SetBody(parseStatement());
        sematicAnalyzer.FinishLastLoopProcessing();
        return res;
    }
    auto iteration = parseExpr();
    requireNext(TokenType::RBRACKET);
    res = new ForStatementNode(init, condition, iteration, nullptr);
    sematicAnalyzer.ProcessLoop(res);
    res->SetBody(parseStatement());
    sematicAnalyzer.FinishLastLoopProcessing();
    return res;
}

//while (expr) statement

WhileStatementNode *Parser::parseWhileStatement()
{
    requireKeywordNext(Keyword::WHILE);
    requireNext(TokenType::LBRACKET);
    auto condition = parseExpr();
    sematicAnalyzer.CheckSelectionOrIterationStatementCondition(condition);
    requireNext(TokenType::RBRACKET);
    auto res = new WhileStatementNode(condition, nullptr);
    sematicAnalyzer.ProcessLoop(res);
    res->SetBody(parseStatement());
    sematicAnalyzer.FinishLastLoopProcessing();
    return res;
}

//do statement while (expr) ;

DoWhileStatementNode *Parser::parseDoWhileStatement()
{
    requireKeywordNext(Keyword::DO);
    auto res = new DoWhileStatementNode(nullptr, nullptr);
    sematicAnalyzer.ProcessLoop(res);
    res->SetBody(parseStatement());
    sematicAnalyzer.FinishLastLoopProcessing();
    requireKeywordNext(Keyword::WHILE);
    requireNext(TokenType::LBRACKET);
    auto condition = parseExpr();
    sematicAnalyzer.CheckSelectionOrIterationStatementCondition(condition);
    res->SetCondition(condition);
    requireNext(TokenType::RBRACKET);
    requireNext(TokenType::SEMICOLON);
    return res;
}

void Parser::parsePointer(DeclaratorNode *declarator)
{
    if (!maybeNext(TokenType::ASTERIX)) return;
    auto quals = TypeBuilder::BuildTypeQualifiers(parseTypeQualifierList()); // TODO that's what SymPointer gonna store
    if (quals)
        declarator->SetType(new SymQualifiedType(new SymPointer(declarator->GetType()), quals));
    else
        declarator->SetType(new SymPointer(declarator->GetType()));
    parsePointer(declarator);
}

//declarator ::= `pointer direct-declarator

void Parser::parseDeclarator(DeclaratorKind kind, DeclaratorNode *declarator)
{
    parsePointer(declarator);
    parseDirectDeclarator(kind, declarator);
}

//direct-declarator ::= id | (declarator)
//                      | direct-declarator [constant-expr]
//                      | direct-declarator (`parameter-type-list)

void Parser::parseDirectDeclarator(DeclaratorKind kind, DeclaratorNode *declarator)
{
    bool gotId = false;
    SymType *lastType = nullptr;
    DeclaratorNode *newDecl = nullptr;
    if (maybeNext(TokenType::LBRACKET))
    {
        newDecl = new DeclaratorNode();
        parseDeclarator(kind, newDecl);
        auto type = newDecl->GetType();

        while (type)
        {
            switch (type->GetTypeKind())
            {
                case TypeKind::POINTER:
                    lastType = type;
                    type = ((SymPointer *)type)->GetTarget();
                    break;
                case TypeKind::ARRAY:
                    lastType = type;
                    type = ((SymArray *)type)->GetValueType();
                    break;
                case TypeKind::FUNCTION:
                    lastType = type;
                    type = ((SymFunction *)type)->GetReturnType();
                    break;
            }
        }
        gotId = true;
        if (newDecl) declarator->SetId(newDecl->GetId());
        requireNext(TokenType::RBRACKET);
    }

    if (kind == DeclaratorKind::NORMAL && !gotId) require(TokenType::ID);
    if (kind != DeclaratorKind::ABSTRACT && maybe(TokenType::ID))
    {
        declarator->SetId(new IdNode(scanner->Current()));
        scanner->Next();
    }
    while (maybe(TokenType::LSQUARE_BRACKET) || maybe(TokenType::LBRACKET))
    {
        if (maybe(TokenType::LSQUARE_BRACKET))
            parseArrayDeclarator(declarator);
        else
            parseFunctionDeclarator(declarator);
    }
    if (lastType)
    {
        switch (lastType->GetTypeKind())
        {
            case TypeKind::POINTER:
                ((SymPointer *)lastType)->SetTarget(declarator->GetType());
                break;
            case TypeKind::ARRAY:
                ((SymArray *)lastType)->SetValueType(declarator->GetType());
                break;
            case TypeKind::FUNCTION:
                ((SymFunction *)lastType)->SetReturnType(declarator->GetType());
                break;
        }
        declarator->SetType(newDecl->GetType());
    }

}


void Parser::parseFunctionDeclarator(DeclaratorNode *declarator)
{
    scanner->Next();
    auto ptl = parseParameterTypeList();
    std::vector<SymVariable *> orderedParamTypes;
    if (ptl->List().size() == 1)
    {
        auto bit = dynamic_cast<SymBuiltInType *>(ptl->List().front()->GetType()->GetUnqualified());
        if (bit && bit->GetBuiltInTypeKind() == BuiltInTypeKind::VOID)
            ptl->List().clear();
    }
    orderedParamTypes.reserve(ptl->List().size());
    auto table = new SymbolTable();
    for (auto it: ptl->List())
    {
        std::string name = it->GetId() ? it->GetId()->GetName() : "#" + std::to_string(orderedParamTypes.size());

        auto var = new SymVariable(name, (*it).GetType(), (*it).GetId());
        var->SetName(name);
        orderedParamTypes.push_back(var);
        table->Insert(name, var);
    }
    table->SetParent(sematicAnalyzer.GetScopeTree()->GetActiveScope());
    requireNext(TokenType::RBRACKET);
    declarator->SetType(new SymFunction(declarator->GetType(), table, orderedParamTypes));
}

void Parser::parseArrayDeclarator(DeclaratorNode *declarator)
{
    requireNext(TokenType::LSQUARE_BRACKET);
    if (maybeNext(TokenType::RSQUARE_BRACKET))
    {
        declarator->SetType(new SymArray(declarator->GetType(), nullptr));
        return;
    }
    auto ce = sematicAnalyzer.EvaluateArraySizer(parseConstantExpr());
//    auto ce = parseConstantExpr();
    requireNext(TokenType::RSQUARE_BRACKET);
    declarator->SetType(new SymArray(declarator->GetType(), ce));
}

//pointer ::= * `type-qualifier-list | * `type-qualifier-list pointer

PointerNode *Parser::parsePointer()
{
    if (!maybeNext(TokenType::ASTERIX)) return nullptr;
    return new PointerNode(parseTypeQualifierList(), parsePointer());
}

//argument-expr-list ::= argument-expr | argument-expr-list, argument-expr

ArgumentExprListNode *Parser::parseArgumentExprList()
{
    auto ael = new ArgumentExprListNode();
    while (!maybe(TokenType::RBRACKET))
    {
        ael->Add(parseAssignmentExpr());
        if (!maybe(TokenType::RBRACKET))
            requireNext(TokenType::COMMA);
    }
    return ael;
}

//storage-class-specifier ::= typedef | extern | static | auto | register

bool Parser::isStorageClassSpecifier(std::shared_ptr<Token> token)
{
    return token->type == TokenType::KEYWORD ?
           StorageClassSpecifiers.find(token->keyword) != StorageClassSpecifiers.end() : false;
}

//function-specifier ::= inline

bool Parser::isFunctionSpecifier(std::shared_ptr<Token> token)
{
    return maybeKeyword(Keyword::INLINE);
}

//parameter-declaration ::= declaration-specifiers declarator | declaration-specifiers `abstract-declarator

ParameterDeclarationNode *Parser::parseParameterDeclaration()
{
    auto declarator = new DeclaratorNode();
    declarator->SetType(TypeBuilder::Build(parseDeclarationSpecifiers()));
    parseDeclarator(DeclaratorKind::ABSTRACT_OR_NORMAL, declarator);
    auto type = declarator->GetType()->GetUnqualified();
    if (type->GetTypeKind() == TypeKind::ARRAY)
        declarator->SetType(new SymPointer(reinterpret_cast<SymArray *>(type)->GetValueType()));
    return new ParameterDeclarationNode(declarator);
}

//declaration-specifiers ::= storage-class-specifier `declaration-specifier |
//| type-specifier `declaration-specifier
//| type-qualifier `declaration-specifier
//| function-specifier declaration-specifier

DeclarationSpecifiersNode *Parser::parseDeclarationSpecifiers()
{
    auto ds = new DeclarationSpecifiersNode();
    while(isDeclarationSpecifier(scanner->Current()))
    {
        if (maybe(TokenType::ID) && isTypedefIdentifier(scanner->Current()))
        {
            ds->Add(sematicAnalyzer.BuildTypedefIdentifierNode(scanner->Current()));
            scanner->Next();
        }
        else if (maybeKeyword(Keyword::STRUCT))
            ds->Add((DeclarationSpecifierNode *)parseStructSpecifier());
        else if (maybeKeyword(Keyword::ENUM))
            ds->Add((DeclarationSpecifierNode *)parseEnumSpecifier());
        else
        {
            ds->Add(new SimpleSpecifier(scanner->Current()));
            scanner->Next();
        }
    }
    if (!ds->Size()) throw NoDeclarationSpecifiers(scanner->Current());
    return ds;
}

//parameter-list ::= parameter-declaration | parameter-list , parameter-declaration

ParameterList *Parser::parseParameterList()
{
    auto pl = new ParameterList();
    while (!maybe(TokenType::RBRACKET))
    {
        pl->Add(parseParameterDeclaration());
        if (!maybe(TokenType::RBRACKET))
            requireNext(TokenType::COMMA);
    }
    return pl;
}

//parameter-type-list ::= parameter-list

ParameterList *Parser::parseParameterTypeList()
{
    return parseParameterList();
}

//declaration-specifiers ::= storage-class-specifier `declaration-specifier |
//| type-specifier `declaration-specifier
//| type-qualifier `declaration-specifier
//| function-specifier declaration-specifier

bool Parser::isDeclarationSpecifier(std::shared_ptr<Token> token)
{
    return isTypeSpecifier(token) || isStorageClassSpecifier(token) || isTypeQualifier(token)
           || isFunctionSpecifier(token);
}

//declaration ::= declaration-specifiers `init-declarator-list ;

DeclarationNode * Parser::parseDeclaration(DeclarationSpecifiersNode *declarationSpecifiers,
                                           InitDeclaratorNode *declarator,
                                           bool isTypedef)
{

    auto ds = declarationSpecifiers ? declarationSpecifiers : parseDeclarationSpecifiers();
    if (maybe(TokenType::SEMICOLON) && !declarator)
    {
        scanner->Next();
        return new DeclarationNode(ds, nullptr);
    }
    auto idl = parseInitDeclaratorList(ds, declarator);
    requireNext(TokenType::SEMICOLON);
    return new DeclarationNode(ds, idl);
}

//init-declarator-list ::= init-declarator | init-declarator-list , init-declarator

InitDeclaratorListNode *Parser::parseInitDeclaratorList(DeclarationSpecifiersNode *declarationSpecifiers,
                                                        InitDeclaratorNode *declarator,
                                                        bool isTypeDef)
{
    auto idl = new InitDeclaratorListNode();
    if (declarator)
    {

        idl->Add(declarator);
        if (!maybe(TokenType::COMMA)) return idl;
        scanner->Next();
    }
    auto type = TypeBuilder::Build(declarationSpecifiers);
    do
    {
        idl->Add(parseInitDeclarator(type, isTypeDef));
    } while (maybeNext(TokenType::COMMA));
    return idl;
}

//init-declarator ::= declarator | declaratxor = initializer

InitDeclaratorNode *Parser::parseInitDeclarator(SymType *type, bool isTypeDef)
{
    // TODO pass built type instead of building every time
    auto declarator = new DeclaratorNode();
    declarator->SetType(type);
    parseDeclarator(DeclaratorKind::NORMAL, declarator);
    InitializerNode *initializer = nullptr;
    if (maybeNext(TokenType::ASSIGNMENT))
        initializer = parseInitializer();
    return sematicAnalyzer.BuildInitDeclaratorNode(declarator, initializer, isTypeDef);
}

//labeled-statement ::= id : statement

LabelStatementNode *Parser::parseLabelStatement()
{
    require(TokenType::ID);
    auto id = new IdNode(scanner->Current());
    scanner->Next();
    requireNext(TokenType::COLON);
    return new LabelStatementNode(id, parseStatement());
}

//compound-statement ::= {`block-item-list}

CompoundStatement *Parser::parseCompoundStatement()
{

    require(TokenType::LCURLY_BRACKET);
    sematicAnalyzer.GetScopeTree()->StartScope();
    if (scanner->Next()->type == TokenType::RCURLY_BRACKET && scanner->Next())
    {
        sematicAnalyzer.GetScopeTree()->EndScope();
        return new CompoundStatement(nullptr);
    }
    auto blockItemList = parseBlockItemList();
    requireNext(TokenType::RCURLY_BRACKET);
    sematicAnalyzer.GetScopeTree()->EndScope();
    return new CompoundStatement(blockItemList);
}

//block-item-list ::= block-item | block-item-list block-item

BlockItemListNode *Parser::parseBlockItemList()
{
    auto blockItemList = new BlockItemListNode();
    while (!maybe(TokenType::RCURLY_BRACKET))
    {
        blockItemList->Add(parseBlockItem());
    }
    return blockItemList;
}

//block-item ::= declaration | statement

BlockItemNode *Parser::parseBlockItem()
{
    return (isDeclarationSpecifier(scanner->Current()) ? (BlockItemNode *)parseDeclaration() : (BlockItemNode *)parseStatement());
}

//enum-specifier ::= enum `id {enumerator-list}
//| enum `id {enumerator-list , }
//| enum id

EnumSpecifierNode *Parser::parseEnumSpecifier()
{
    IdNode *id = nullptr;
    EnumeratorList *list = nullptr;
    requireKeyword(Keyword::ENUM);
    if (scanner->Next()->type == TokenType::ID)
    {
        id = new IdNode(scanner->Current());
        scanner->Next();
    }
    if (!id) require(TokenType::LCURLY_BRACKET);
    if (maybeNext(TokenType::LCURLY_BRACKET))
    {
        list = parseEnumeratorList();
        if (!list->Size()) throw EmptyEnumeratorListError(scanner->Current());
        requireNext(TokenType::RCURLY_BRACKET);
    }
    return sematicAnalyzer.BuildEnumSpecifierNode(id, list);
}

//enumerator-list ::= enumerator | enumerator-list , enumerator

EnumeratorList *Parser::parseEnumeratorList()
{
    auto list = new EnumeratorList();
    do
    {
        if (maybe(TokenType::RCURLY_BRACKET)) break;
        list->Add(parseEnumerator());
    } while(maybeNext(TokenType::COMMA));
    return list;
}

//enumerator ::= enumeration-constant, enumeration-constant = constant-expr

EnumeratorNode *Parser::parseEnumerator()
{
    require(TokenType::ID);
    auto id = new IdNode(scanner->Current());
    scanner->Next();
    if (maybeNext(TokenType::ASSIGNMENT))
        return sematicAnalyzer.BuildEnumeratorNode(id, parseConstantExpr());
    return sematicAnalyzer.BuildEnumeratorNode(id, nullptr);
}

// a specifier that consists of 1 keyword: type-qualifier | type-specifier
// | storage-class-specifier | function-specifier

bool Parser::isSimpleSpecifier(std::shared_ptr<Token> token)
{
    return isTypeQualifier(token) || isTypeSpecifier(token) ||
            isStorageClassSpecifier(token) || isFunctionSpecifier(token);
}

//struct-specifier ::= struct `id {struct-declaration-list}
//| struct id

StructSpecifierNode *Parser::parseStructSpecifier()
{
    auto stoken = scanner->Current();
    requireKeywordNext(Keyword::STRUCT);
    IdNode *id = maybe(TokenType::ID) ? new IdNode(scanner->Current()) : nullptr;
    if (!id)
    {
        requireNext(TokenType::LCURLY_BRACKET);
        return sematicAnalyzer.BuildStructSpecifierNode(id, parseStructDeclarationList(), stoken);
    }
    scanner->Next();
    if (maybeNext(TokenType::LCURLY_BRACKET))
        return sematicAnalyzer.BuildStructSpecifierNode(id, parseStructDeclarationList(), stoken);
    return sematicAnalyzer.BuildStructSpecifierNode(id, nullptr, stoken);
}

//struct-declaration-list ::= struct-declaration | struct-declaration-list struct-declaration

StructDeclarationListNode *Parser::parseStructDeclarationList()
{
    auto sdl = new StructDeclarationListNode();
    do
    {
        sdl->Add(parseStructDeclaration());
    } while (!maybe(TokenType::RCURLY_BRACKET));
    scanner->Next();
    return sdl;
}

//struct-declaration ::= specifier-qualifier-list struct-declarator-list ;

StructDeclarationNode *Parser::parseStructDeclaration()
{
    auto structDecl = new StructDeclarationNode(
            parseStructDeclaratorList(TypeBuilder::Build(parseSpecifierQualifierList())));
    requireNext(TokenType::SEMICOLON);
    return structDecl;
}

//struct-declarator ::= declarator | `declarator : constant-expr

StructDeclaratorNode *Parser::parseStructDeclarator(SymType *baseType)
{
    if (maybeNext(TokenType::COLON))
        return new StructDeclaratorNode(nullptr, parseConstantExpr());
    auto declarator = new DeclaratorNode();
    declarator->SetType(baseType);
    parseDeclarator(DeclaratorKind::NORMAL, declarator);
    if (maybeNext(TokenType::COLON))
        return new StructDeclaratorNode(declarator, parseConstantExpr());
    return new StructDeclaratorNode(declarator, nullptr);
}

//struct-declarator-list ::= struct-declarator | struct-declarator-list , struct-declarator

StructDeclaratorListNode *Parser::parseStructDeclaratorList(SymType *baseType)
{
    auto sdl = new StructDeclaratorListNode();
    do
    {
        sdl->Add(parseStructDeclarator(baseType));
    } while (maybeNext(TokenType::COMMA));
    return sdl;
}

//specifier-qualifier-list ::= type-specifier `specifier-qualifier-list
//| type-qualifier `specifier-qualifier-list

DeclarationSpecifiersNode *Parser::parseSpecifierQualifierList()
{
    auto tnn = new DeclarationSpecifiersNode();
    bool spec;
    while(isTypeQualifier(scanner->Current()) || isTypeSpecifier(scanner->Current()))
    {
        if (maybe(TokenType::ID) && isTypedefIdentifier(scanner->Current()))
        {
            tnn->Add(sematicAnalyzer.BuildTypedefIdentifierNode(scanner->Current()));
            scanner->Next();
        }
        else if (maybeKeyword(Keyword::STRUCT))
            tnn->Add(parseStructSpecifier());
        else if (maybeKeyword(Keyword::ENUM))
            tnn->Add(parseEnumSpecifier());
        else
        {
            tnn->Add(new SimpleSpecifier(scanner->Current()));
            scanner->Next();
        }
    }
    if (tnn->List().empty()) throw NoDeclarationSpecifiers(scanner->Current());
    return tnn;
}

//initializer ::= assignment-expr | {initializer-list} | {initializer-list , }

InitializerNode *Parser::parseInitializer()
{
    if (maybe(TokenType::LCURLY_BRACKET))
    {
        auto il =  (InitializerNode *)parseInitializerList();
        requireNext(TokenType::RCURLY_BRACKET);
        return il;
    }
    return new SimpleInitializer(parseAssignmentExpr());
}

//initializer-list ::= `designation initializer | initializer-list , `designation initializer

InitializerListNode *Parser::parseInitializerList()
{
    requireNext(TokenType::LCURLY_BRACKET);
    auto il = new InitializerListNode();
    do
    {
        if (maybe(TokenType::RCURLY_BRACKET)) break;
        il->Add(parseDesignatedInitializer());
    } while (maybeNext(TokenType::COMMA));
    return il;
}

//designation ::= designator-list =

DesignationNode *Parser::parseDesignation()
{
    auto designatorList = parseDesignatorList();
    requireNext(TokenType::ASSIGNMENT);
    return new DesignationNode(designatorList);
}

//designator-list ::= designator | designator-list designator

DesignatorListNode *Parser::parseDesignatorList()
{
    auto dl = new DesignatorListNode();
    do
    {
        dl->Add(parseDesignator());
    } while(maybe(TokenType::LSQUARE_BRACKET) || maybe(TokenType::DOT));
    return dl;
}

//designator ::= [constant-expr] | . id

DesignatorNode *Parser::parseDesignator()
{
    if (maybeNext(TokenType::LSQUARE_BRACKET))
    {
        auto constExpr = parseConstantExpr();
        requireNext(TokenType::RSQUARE_BRACKET);
        return new ArrayDesignator(constExpr);
    }
    requireNext(TokenType::DOT);
    require(TokenType::ID);
    auto smd = new StructMemberDesignator(new IdNode(scanner->Current()));
    scanner->Next();
    return smd;
}

// `designation initializer

InitializerNode *Parser::parseDesignatedInitializer()
{
    if (maybe(TokenType::DOT) || maybe(TokenType::LSQUARE_BRACKET))
        return new DesignatedInitializerNode(parseDesignation(), parseInitializer());
    return parseInitializer();
}

//translation-unit ::= external-declaration | translation-unit external-declaration

TranslationUnitNode *Parser::parseTranslationUnit()
{
    auto tu = new TranslationUnitNode();
    while (!maybe(TokenType::END_OF_FILE))
        tu->Add(parseExternalDeclaration());
    return tu;
}

//external-declaration ::= function-definition | declaration

ExternalDeclarationNode *Parser::parseExternalDeclaration()
{
    auto ds = parseDeclarationSpecifiers();
    auto isTypedef = false;
    auto type = TypeBuilder::Build(ds, isTypedef);
    if (maybeNext(TokenType::SEMICOLON))
        return (ExternalDeclarationNode *)new DeclarationNode(ds, nullptr);
    auto declarator = new DeclaratorNode();
    declarator->SetType(type);
    parseDeclarator(DeclaratorKind::NORMAL, declarator);
    if (maybe(TokenType::LCURLY_BRACKET))
    {
        auto res = sematicAnalyzer.BuildFunctionDefinitionNode(declarator, nullptr);
        auto body = parseCompoundStatement();
        res->SetBody(body);
        sematicAnalyzer.GetScopeTree()->EndScope();
        sematicAnalyzer.FinishLastFunctionProcessing();
        return res;
    }
    if (maybeNext(TokenType::ASSIGNMENT))
        return parseDeclaration(ds, sematicAnalyzer.BuildInitDeclaratorNode(declarator, parseInitializer(), isTypedef));
    return parseDeclaration(ds, sematicAnalyzer.BuildInitDeclaratorNode(declarator, nullptr, isTypedef));
}

void Parser::require(TokenType typeExpectation)
{
    if (scanner->Current()->type != typeExpectation) throw UnexpectedTokenError(scanner->Current(), typeExpectation);
}

void Parser::requireKeyword(Keyword expectedKeyword)
{
    if (scanner->Current()->type != TokenType::KEYWORD || scanner->Current()->keyword != expectedKeyword)
        throw UnexpectedKeywordError(scanner->Current(), expectedKeyword);
}

void Parser::requireNext(TokenType typeExpectation)
{
    require(typeExpectation);
    scanner->Next();
}

void Parser::requireKeywordNext(Keyword expectedKeyword)
{
    requireKeyword(expectedKeyword);
    scanner->Next();
}

bool Parser::maybe(TokenType typeExpectation)
{
    return scanner->Current()->type == typeExpectation;
}

bool Parser::maybeNext(TokenType typeExpectation)
{
    return maybe(typeExpectation) && scanner->Next();
}

bool Parser::isTypedefIdentifier(const std::shared_ptr<Token> &token)
{
    if (token->type != TokenType::ID) return false;
    auto sym = sematicAnalyzer.GetScopeTree()->Find(token->text);
    return dynamic_cast<SymAlias *>(sym);
}

bool Parser::maybeKeywordNext(Keyword keyword)
{
    return maybeKeyword(keyword) && scanner->Next();
}

bool Parser::maybeKeyword(Keyword keyword)
{
    return scanner->Current()->type == TokenType::KEYWORD && scanner->Current()->keyword == keyword;
}

void Parser::Generate(Asm::Assembly *assembly)
{
    tree.root->Generate(assembly);
}
