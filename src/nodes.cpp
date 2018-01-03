//
// Created by keltar on 10/14/17.
//

#include "../includes/nodes.h"
#include "../includes/evaluator.h"

void IntConstNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value << std::endl;
}

IntConstNode::IntConstNode(std::shared_ptr<Token> token)
{
    if (token->type != TokenType::NUM_INT) throw "";
    type = new SymBuiltInType(BuiltInTypeKind::INT32, 0);
    position = token->position;
    value = token->intValue;
}

ExprNode *IntConstNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

int32_t IntConstNode::GetValue() const
{
    return value;
}

IntConstNode::IntConstNode(int32_t value)
{
    this->value = value;
    type = new SymBuiltInType(BuiltInTypeKind::INT32);
}

void IntConstNode::Generate(Asm::Assembly *assembly)
{
    assembly->AddCommand(Asm::CommandName::PUSH, (ConstNode *)this, Asm::CommandSuffix::L);
    // TODO
}

void FloatConstNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value << std::endl;
}

FloatConstNode::FloatConstNode(std::shared_ptr<Token> token)
{
    if (token->type != TokenType::NUM_FLOAT) throw "";
    type = new SymBuiltInType(BuiltInTypeKind::FLOAT, 0);
    position = token->position;
    value = token->floatValue;
}

ExprNode *FloatConstNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

float FloatConstNode::GetValue() const
{
    return value;
}

void FloatConstNode::Generate(Asm::Assembly *assembly)
{
    assembly->AddCommand(Asm::CommandName::PUSH, (ConstNode *)this, Asm::CommandSuffix::L);
    // TODO
}

void IdNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << token->text << std::endl;
}

IdNode::IdNode(std::shared_ptr<Token> token): token(token)
{
    if (token->type != TokenType::ID) throw "";
    category = ValueCategory::LVAVLUE;
    position = token->position;
}

std::string IdNode::GetName() const
{
    return token->stringValue;
}

IdNode::IdNode(std::shared_ptr<Token> token, SymType *type): IdNode(token)
{
    this->type = type;
    position = token->position;
}

ExprNode *IdNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void IdNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StringLiteralNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << token->text << std::endl;
}

StringLiteralNode::StringLiteralNode(std::shared_ptr<Token> token): token(token)
{
    if (token->type != TokenType::STRING) throw "";
    position = token->position;
}

ExprNode *StringLiteralNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void StringLiteralNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void PostfixIncrementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "`++" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    node->Print(os, indent, true);
}

PostfixIncrementNode::PostfixIncrementNode(ExprNode *node) : node(node)
{
    this->type = node->GetType();
}

ExprNode * PostfixIncrementNode::Eval(Evaluator *evaluator)
{
    evaluator->Eval(this);
}

void PostfixIncrementNode::Generate(Asm::Assembly *assembly)
{
    
}

void PostfixDecrementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "`--" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    node->Print(os, indent, true);
}

PostfixDecrementNode::PostfixDecrementNode(ExprNode *node) : node(node)
{
    this->type = node->GetType();
}

ExprNode *PostfixDecrementNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void PostfixDecrementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructureOrUnionMemberAccessNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "." << std::endl;
    indent.append(isTail ? "    " : "│   ");
    structureOrUnion->Print(os, indent, false);
    member->Print(os, indent, true);
}

StructureOrUnionMemberAccessNode::StructureOrUnionMemberAccessNode(ExprNode *structureOrUnion, IdNode *member) :
        member(member), structureOrUnion(structureOrUnion) {}

ExprNode *StructureOrUnionMemberAccessNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void StructureOrUnionMemberAccessNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructureOrUnionMemberAccessByPointerNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "->" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    structureOrUnion->Print(os, indent, false);
    member->Print(os, indent, true);
}

StructureOrUnionMemberAccessByPointerNode::StructureOrUnionMemberAccessByPointerNode(ExprNode *structureOrUnion,
                                                                                     IdNode *member) : member(member),
                                                                                                       structureOrUnion(structureOrUnion) {}

ExprNode *StructureOrUnionMemberAccessByPointerNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void StructureOrUnionMemberAccessByPointerNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void PrefixIncrementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "++'" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    node->Print(os, indent, true);
}

PrefixIncrementNode::PrefixIncrementNode(ExprNode *node) : node(node)
{
    this->type = node->GetType();
}

ExprNode *PrefixIncrementNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void PrefixIncrementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void PrefixDecrementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "--`" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    node->Print(os, indent, true);
}

PrefixDecrementNode::PrefixDecrementNode(ExprNode *node) : node(node)
{
    this->type = node->GetType();
}

ExprNode *PrefixDecrementNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void PrefixDecrementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void BinOpNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << op->text << std::endl;
    indent.append(isTail ? "    " : "│   ");
    left->Print(os, indent, false);
    right->Print(os, indent, true);
}

BinOpNode::BinOpNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> op) : left(left), right(right), op(std::move(op))
{
    this->type = left->GetType();
    this->position = this->op->position;
}

BinOpNode::BinOpNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> op, SymType *resultType):
        BinOpNode(left, right, std::move(op))
{
    this->type = resultType;
}

ExprNode *BinOpNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

ExprNode *BinOpNode::Left() const
{
    return left;
}

ExprNode *BinOpNode::Right() const
{
    return right;
}

std::shared_ptr<Token> BinOpNode::GetOperation() const
{
    return op;
}

void BinOpNode::Generate(Asm::Assembly *assembly)
{
    left->Generate(assembly);
    right->Generate(assembly);
    switch (op->type)
    {
        case TokenType::PLUS:
            if (left->GetType()->GetTypeKind() == TypeKind::BUILTIN)
            {
                switch (reinterpret_cast<SymBuiltInType *>(left->GetType())->GetBuiltIntTypeKind())
                {
                    case BuiltInTypeKind::INT32:
                        assembly->AddCommand(Asm::CommandName::POP, Asm::Register::EBX, Asm::CommandSuffix::L);
                        assembly->AddCommand(Asm::CommandName::POP, Asm::Register::EAX, Asm::CommandSuffix::L);
                        assembly->AddCommand(Asm::CommandName::ADD, Asm::Register::EBX, Asm::Register::EAX, Asm::CommandSuffix::L);
                        assembly->AddCommand(Asm::CommandName::PUSH, Asm::Register::EAX, Asm::CommandSuffix::L);
                        break;
                    case BuiltInTypeKind::FLOAT:
                        assembly->AddCommand(Asm::CommandName::FLD,
                                             Asm::MakeAddress(Asm::Registers[Asm::Register::ESP]),
                                             Asm::CommandSuffix::S);
                        assembly->AddCommand(Asm::CommandName::POP, Asm::Register::EAX,
                                             Asm::CommandSuffix::L);
                        assembly->AddCommand(Asm::CommandName::FLD,
                                             Asm::MakeAddress(Asm::Registers[Asm::Register::ESP]),
                                             Asm::CommandSuffix::S);
                        assembly->AddCommand(Asm::CommandName::FADDP);
                        assembly->AddCommand(Asm::CommandName::FSTP,
                                             Asm::MakeAddress(Asm::Registers[Asm::Register::ESP]),
                                             Asm::CommandSuffix::S);
                        break;
                }
            }

            break;
        case TokenType::MINUS:
            break;
    }
}

void ArrayAccessNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "[]" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    left->Print(os, indent, false);
    inBrackets->Print(os, indent, true);
}

ArrayAccessNode::ArrayAccessNode(ExprNode *left, ExprNode *inBrackets) : left(left), inBrackets(inBrackets) {}

ExprNode *ArrayAccessNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void ArrayAccessNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void TernaryOperatorNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "?:" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    condition->Print(os, indent, false);
    iftrue->Print(os, indent, false);
    iffalse->Print(os, indent, true);
}

TernaryOperatorNode::TernaryOperatorNode(ExprNode *condition, ExprNode *iftrue, ExprNode *iffalse) :
        condition(condition), iftrue(iftrue), iffalse(iffalse)
{
    this->type = iftrue->GetType();
}

ExprNode *TernaryOperatorNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void TernaryOperatorNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}


void AssignmentNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << assignmentOp->stringValue << std::endl;
    indent.append(isTail ? "    " : "│   ");
    left->Print(os, indent, false);
    right->Print(os, indent, true);
}

AssignmentNode::AssignmentNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> assignmentOp) : left(left), right(right),
                                                                                                       assignmentOp(assignmentOp)
{
    this->type = left->GetType();
}

ExprNode *AssignmentNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void AssignmentNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void TypeCastNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "typcast" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    castType->Print(os, indent, false);
    castExpr->Print(os, indent, true);
}

TypeCastNode::TypeCastNode(SymType *type, ExprNode *castExpr) : castType(type), castExpr(castExpr)
{
    this->type = type;
}

ExprNode *TypeCastNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void TypeCastNode::Generate(Asm::Assembly *assembly)
{
    castExpr->Generate(assembly);
    auto cet = castExpr->GetType();
    if (cet->GetTypeKind() == TypeKind::BUILTIN && castType->GetTypeKind() == TypeKind::BUILTIN)
    {
        auto castTypeBtk = reinterpret_cast<SymBuiltInType *>(castType);
        auto castExprBtk = reinterpret_cast<SymBuiltInType *>(cet);
        BuiltInTypeConversions[std::make_pair(castExprBtk->GetBuiltIntTypeKind(), castTypeBtk->GetBuiltIntTypeKind())](assembly);
    }
    // TODO
}

void UnaryOpNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << unaryOp->stringValue << std::endl;
    indent.append(isTail ? "    " : "│   ");
    expr->Print(os, indent, true);
}

UnaryOpNode::UnaryOpNode(std::shared_ptr<Token> unaryOp, ExprNode *expr) : unaryOp(unaryOp), expr(expr)
{
    position = unaryOp->position;
}

ExprNode *UnaryOpNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void UnaryOpNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void SizeofExprNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "sizeof" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    expr->Print(os, indent, true);
}

SizeofExprNode::SizeofExprNode(ExprNode *expr) : expr(expr) {}

ExprNode *SizeofExprNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void SizeofExprNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void SizeofTypeNameNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "sizeof()" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    typeName->Print(os, indent, true);
}

SizeofTypeNameNode::SizeofTypeNameNode(SymType *typeName) : typeName(typeName) {}

ExprNode *SizeofTypeNameNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void SizeofTypeNameNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void CommaSeparatedExprs::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "," << std::endl;
    indent.append(isTail ? "    " : "│   ");
    left->Print(os, indent, false);
    right->Print(os, indent, true);
}

CommaSeparatedExprs::CommaSeparatedExprs(ExprNode *left, ExprNode *right) : left(left), right(right) {}

ExprNode *CommaSeparatedExprs::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void CommaSeparatedExprs::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void PointerNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "*" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (pointer)
    {
        typeQualifierList->Print(os, indent, false);
        pointer->Print(os, indent, true);
    }
    else
        typeQualifierList->Print(os, indent, true);
}

PointerNode::PointerNode(DeclarationSpecifiersNode *typeQualifierList, PointerNode *pointer) :
        typeQualifierList(typeQualifierList), pointer(pointer) {}

void PointerNode::Generate(Asm::Assembly *assembly)
{

}

void ExprStatmentNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << ";" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (expr) expr->Print(os, indent, true);
}

ExprStatmentNode::ExprStatmentNode(ExprNode *expr) : expr(expr) {}

void ExprStatmentNode::Generate(Asm::Assembly *assembly)
{
    expr->Generate(assembly);
}

void IfStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "if" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    expr->Print(os, indent, false);
    then->Print(os, indent, true);
}

void IfStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

IfStatementNode::IfStatementNode(ExprNode *expr, StatementNode *then) : expr(expr), then(then) {}

void IfElseStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "if else" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    expr->Print(os, indent, false);
    then->Print(os, indent, false);
    _else->Print(os, indent, true);
}

IfElseStatementNode::IfElseStatementNode(ExprNode *expr, StatementNode *then, StatementNode *_else) :
        IfStatementNode(expr, then), _else(_else) {}

void IfElseStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void GotoStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "goto" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    id->Print(os, indent, true);
}

GotoStatementNode::GotoStatementNode(IdNode *id) : id(id) {}

void GotoStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ContinueStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "continue" << std::endl;
}

void ContinueStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void BreakStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "break" << std::endl;
}

void BreakStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ReturnStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "return" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (expr) expr->Print(os, indent, true);
}

ReturnStatementNode::ReturnStatementNode(ExprNode *expr) : expr(expr) {}

void ReturnStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void WhileStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "while" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    condition->Print(os, indent, false);
    body->Print(os, indent, true);
}

WhileStatementNode::WhileStatementNode(ExprNode *condition, StatementNode *body): condition(condition)
{
    this->body = body;
}

void WhileStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DoWhileStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "do" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    body->Print(os, indent, false);
    condition->Print(os, indent, true);
}

DoWhileStatementNode::DoWhileStatementNode(ExprNode *condition, StatementNode *body): condition(condition)
{
    this->body = body;
}

void DoWhileStatementNode::SetCondition(ExprNode *condition)
{
    this->condition = condition;
}

void DoWhileStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ForStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "for" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    init->Print(os, indent, false);
    condition->Print(os, indent, false);

    if (iteration)
        iteration->Print(os, indent, false);
    else
        os << indent << "├── " << std::endl;
    body->Print(os, indent, true);

}

ForStatementNode::ForStatementNode(ExprStatmentNode *init, ExprStatmentNode *condition, ExprNode *iteration,
                                   StatementNode *body) : init(init), condition(condition), iteration(iteration)
{
    this->body = body;
}

void ForStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}


void LabelStatementNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "label" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    labelName->Print(os, indent, false);
    statement->Print(os, indent, true);
}

LabelStatementNode::LabelStatementNode(IdNode *labelName, StatementNode *statement) : labelName(labelName), statement(statement) {}

void LabelStatementNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DeclaratorNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "Declarator" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (id) id->Print(os, indent, false);
    type->Print(os, indent, true);
}

void DeclaratorNode::SetType(SymType *type)
{
    this->type = type;
}

SymType *DeclaratorNode::GetType() const
{
    return type;
}

void DeclaratorNode::SetId(IdNode *name)
{
    this->id = name;
}

IdNode *DeclaratorNode::GetId() const
{
    return id;
}

void DeclaratorNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ArgumentExprListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "ArgumentExprList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void ArgumentExprListNode::Add(ExprNode *assignmentExpr)
{
    list.push_back(assignmentExpr);
}

uint64_t ArgumentExprListNode::Size()
{
    return list.size();
}

std::list<ExprNode *> &ArgumentExprListNode::List()
{
    return list;
}

void ArgumentExprListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void FunctionCallNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "FuncCall" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (arguments->Size())
    {
        function->Print(os, indent, false);
        arguments->Print(os, indent, true);
        return;
    }
    function->Print(os, indent, true);
}

FunctionCallNode::FunctionCallNode(ExprNode *function, ArgumentExprListNode *arguments) :
        function(function), arguments(arguments) {}

ExprNode *FunctionCallNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void FunctionCallNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DeclarationSpecifiersNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "DeclarationSpecifiers" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void DeclarationSpecifiersNode::Add(DeclarationSpecifierNode *specifier)
{
    list.push_back(specifier);
}

uint64_t DeclarationSpecifiersNode::Size()
{
    return list.size();
}

DeclarationSpecifiersNode::DeclarationSpecifiersNode(std::initializer_list<DeclarationSpecifierNode *> initializerList)
{
    for (auto it : initializerList)
        Add(it);
}

std::list<DeclarationSpecifierNode *> &DeclarationSpecifiersNode::List()
{
    return list;
}

void DeclarationSpecifiersNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ParameterList::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "ParameterList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void ParameterList::Add(ParameterDeclarationNode *parameterDeclaration)
{
    list.push_back(parameterDeclaration);
}

uint64_t ParameterList::Size()
{
    return list.size();
}

std::list<ParameterDeclarationNode *> &ParameterList::List()
{
    return list;
}

void ParameterList::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ParameterDeclarationNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "param" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (GetId()) GetId()->Print(os, indent, false);
    GetType()->Print(os, indent, true);
}

ParameterDeclarationNode::ParameterDeclarationNode(DeclaratorNode *declarator)
{
    SetId(declarator->GetId());
    SetType(declarator->GetType());
}

void ParameterDeclarationNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DeclarationNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "Decl" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (list) list->Print(os, indent, true);
}

void DeclarationNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

uint64_t InitDeclaratorListNode::Size()
{
    return list.size();
}

void InitDeclaratorListNode::Add(InitDeclaratorNode *initDeclarator)
{
    list.push_back(initDeclarator);
}

void InitDeclaratorListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "InitDeclaratorList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

std::list<InitDeclaratorNode *> &InitDeclaratorListNode::List()
{
    return list;
}

void InitDeclaratorListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void InitDeclaratorNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "InitDeclarator" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    GetId()->Print(os, indent, false);
    GetType()->Print(os, indent, !initializer);
    if (initializer) initializer->Print(os, indent, true);
}

InitDeclaratorNode::InitDeclaratorNode(DeclaratorNode *declarator, InitializerNode *initializer): initializer(initializer)
{
    SetType(declarator->GetType());
    SetId(declarator->GetId());
}

InitializerNode *InitDeclaratorNode::GetInitializer() const
{
    return initializer;
}

void InitDeclaratorNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void CompoundStatement::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "{}" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (blockItemList) blockItemList->Print(os, indent, true);
}

CompoundStatement::CompoundStatement(BlockItemListNode *blockItemList) : blockItemList(blockItemList) {}

void CompoundStatement::Generate(Asm::Assembly *assembly)
{
    blockItemList->Generate(assembly);
}

void BlockItemNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << std::endl;
    indent.append(isTail ? "    " : "│   ");
    declOrStatement->Print(os, indent, true);
}

BlockItemNode::BlockItemNode(Node *declOrStatement) : declOrStatement(declOrStatement) {}

void BlockItemNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

uint64_t BlockItemListNode::Size()
{
    return list.size();
}

void BlockItemListNode::Add(BlockItemNode *blockItem)
{
    list.push_back(blockItem);
}

void BlockItemListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "BlockItemList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void BlockItemListNode::Generate(Asm::Assembly *assembly)
{
    for (auto &element: list)
        element->Generate(assembly);
}

uint64_t EnumeratorList::Size()
{
    return list.size();
}

void EnumeratorList::Add(EnumeratorNode *initDeclarator)
{
    list.push_back(initDeclarator);
}

void EnumeratorList::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "EnumeratorList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

std::list<EnumeratorNode *> &EnumeratorList::List()
{
    return list;
}

void EnumeratorList::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void EnumSpecifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "enum" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (id)
    {
        if (enumeratorList)
        {
            id->Print(os, indent, false);
            enumeratorList->Print(os, indent, true);
            return;
        }
        id->Print(os, indent, true);
        return;
    }
    enumeratorList->Print(os, indent, true);
}

EnumSpecifierNode::EnumSpecifierNode(IdNode *id, EnumeratorList *enumeratorList) : id(id), enumeratorList(enumeratorList)
{
    this->kind = SpecifierKind::ENUM;
}

void EnumSpecifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void EnumeratorNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "Enumerator" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (value)
    {
        enumerationConstant->Print(os, indent, false);
        value->Print(os, indent, true);
        return;
    }
    enumerationConstant->Print(os, indent, true);
}

ExprNode *EnumeratorNode::GetValue() const
{
    return value;
}

void EnumeratorNode::SetValue(ExprNode *value)
{
    this->value = value;
}

IdNode *EnumeratorNode::GetId()
{
    return enumerationConstant;
}

EnumeratorNode::EnumeratorNode(IdNode *enumerationConstant, ExprNode *value) :
        enumerationConstant(enumerationConstant), value(value)
{
    this->type = new SymBuiltInType(BuiltInTypeKind::INT32);
}

ExprNode *EnumeratorNode::Eval(Evaluator *evaluator)
{
    return evaluator->Eval(this);
}

void EnumeratorNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void TypeSpecifierQualifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

TypeSpecifierQualifierNode::TypeSpecifierQualifierNode(std::shared_ptr<Token> value) : SimpleSpecifier(std::move(value)) {}

void TypeSpecifierQualifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void TypeSpecifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

TypeSpecifierNode::TypeSpecifierNode(std::shared_ptr<Token> specifier) : TypeSpecifierQualifierNode(std::move(specifier)) {}

void TypeSpecifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void TypeQualifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

TypeQualifierNode::TypeQualifierNode(std::shared_ptr<Token> qualifier) : TypeSpecifierQualifierNode(std::move(qualifier)) {}

void TypeQualifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StorageClassSpecifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

StorageClassSpecifierNode::StorageClassSpecifierNode(std::shared_ptr<Token> specifier) : SimpleSpecifier(std::move(specifier)) {}

void StorageClassSpecifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void FunctionSpecifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

FunctionSpecifierNode::FunctionSpecifierNode(std::shared_ptr<Token> specifier) : SimpleSpecifier(std::move(specifier)) {}

void FunctionSpecifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void SimpleSpecifier::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << value->text << std::endl;
}

std::shared_ptr<Token> SimpleSpecifier::Value()
{
    return value;
}

SimpleSpecifier::SimpleSpecifier(std::shared_ptr<Token> specifier) : value(std::move(specifier)) { kind = SpecifierKind::SIMPLE; }

void SimpleSpecifier::Generate(Asm::Assembly *assembly)
{

}

void StructDeclaratorListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "StructDeclaratorList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void StructDeclaratorListNode::Add(StructDeclaratorNode *initDeclarator)
{
    list.push_back(initDeclarator);
}

uint64_t StructDeclaratorListNode::Size()
{
    return list.size();
}

std::list<StructDeclaratorNode *> &StructDeclaratorListNode::List()
{
    return list;
}

void StructDeclaratorListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructDeclarationNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "StructDeclaratorList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    structDeclaratorList->Print(os, indent, true);
}

std::list<StructDeclaratorNode *> & StructDeclarationNode::List() const
{
    return structDeclaratorList->List();
}

void StructDeclarationNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

uint64_t StructDeclarationListNode::Size()
{
    return list.size();
}

void StructDeclarationListNode::Add(StructDeclarationNode *initDeclarator)
{
    list.push_back(initDeclarator);
}

void StructDeclarationListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "StructDeclarationList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

std::list<StructDeclarationNode *> &StructDeclarationListNode::List()
{
    return list;
}

void StructDeclarationListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructSpecifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "struct" << std::endl;
    indent.append(isTail ? "    " : "│   ");
}

void StructSpecifierNode::SetId(IdNode *id)
{
    this->type->SetTag(id);
}

IdNode *StructSpecifierNode::GetId() const
{
    return this->type->GetTag();
}

SymRecord *StructSpecifierNode::GetRecordType() const
{
    return type;
}

void StructSpecifierNode::SetRecordType(SymRecord *type)
{
    this->type = type;
}

StructSpecifierNode::StructSpecifierNode(SymRecord *type, std::shared_ptr<Token> structToken) : type(type),
                                                                                                token(std::move(structToken))
{
    kind = SpecifierKind::STRUCT;
}

std::shared_ptr<Token> StructSpecifierNode::GetToken() const
{
    return token;
}

void StructSpecifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructDeclaratorNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "StructDeclarator" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (GetId()) GetId()->Print(os, indent, false);
    GetType()->Print(os, indent, constantExpr);
    if (constantExpr) constantExpr->Print(os, indent, true);
}

StructDeclaratorNode::StructDeclaratorNode(DeclaratorNode *declarator, ExprNode *constantExpr): constantExpr(constantExpr)
{
    SetId(declarator->GetId());
    SetType(declarator->GetType());
}

void StructDeclaratorNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

uint64_t InitializerListNode::Size()
{
    return list.size();
}

void InitializerListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "InitializerList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void InitializerListNode::Add(InitializerNode *initializer)
{
    list.push_back(initializer);
}

std::list<InitializerNode *> &InitializerListNode::List()
{
    return list;
}

void InitializerListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DesignatorListNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "DesignatorList" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void DesignatorListNode::Add(DesignatorNode *designator)
{
    list.push_back(designator);
}

uint64_t DesignatorListNode::Size()
{
    return list.size();
}

std::list<DesignatorNode *> &DesignatorListNode::List()
{
    return list;
}

void DesignatorListNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void ArrayDesignator::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "[]" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    index->Print(os, indent, true);
}

ArrayDesignator::ArrayDesignator(ExprNode *index) : index(index) {}

ExprNode *ArrayDesignator::GetIndex() const
{
    return index;
}

void ArrayDesignator::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DesignationNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    designatorList->Print(os, indent, true);
}

DesignationNode::DesignationNode(DesignatorListNode *designatorList) : designatorList(designatorList) {}

std::list<DesignatorNode *> &DesignationNode::List()
{
    return designatorList->List();
}

void DesignationNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void StructMemberDesignator::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "." << std::endl;
    indent.append(isTail ? "    " : "│   ");
    id->Print(os, indent, true);
}

StructMemberDesignator::StructMemberDesignator(IdNode *id) : id(id) {}

IdNode *StructMemberDesignator::GetMemberId() const
{
    return id;
}

void StructMemberDesignator::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DesignatedInitializerNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "DesignatedInitializer" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    if (designation) designation->Print(os, indent, false);
    initializer->Print(os, indent, true);
}

DesignatedInitializerNode::DesignatedInitializerNode(DesignationNode *designation, InitializerNode *initializer) :
        designation(designation), initializer(initializer) {}

DesignationNode *DesignatedInitializerNode::GetDesignation() const
{
    return designation;
}

InitializerNode *DesignatedInitializerNode::GetInitializer() const
{
    return initializer;
}

void DesignatedInitializerNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void FunctionDefinitionNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "FuncDef" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    declarator->Print(os, indent, false);
    body->Print(os, indent, true);
}

void FunctionDefinitionNode::SetBody(CompoundStatement *body)
{
    this->body = body;
}

SymType *FunctionDefinitionNode::GetType() const
{
    return declarator->GetType();
}

FunctionDefinitionNode::FunctionDefinitionNode(DeclaratorNode *declarator, CompoundStatement *compoundStatement) :
        declarator(declarator), body(compoundStatement) {}

void FunctionDefinitionNode::Generate(Asm::Assembly *assembly)
{
    // TODO;
    body->Generate(assembly);
}

void TranslationUnitNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    if (list.empty()) return;
    os << indent << (isTail ? "└── " : "├── ");
    os << "Unit" << std::endl;
    indent.append(isTail ? "    " : "│   ");
    auto it = list.begin();
    if (list.size() > 1)
        for (; it != --list.end(); it++)
            (*it)->Print(os, indent, false);
    (*it)->Print(os, indent, true);
}

void TranslationUnitNode::Add(ExternalDeclarationNode *initDeclarator)
{
    list.push_back(initDeclarator);
}

uint64_t TranslationUnitNode::Size()
{
    return list.size();
}

void TranslationUnitNode::Generate(Asm::Assembly *assembly)
{
    for (auto it = list.begin(); it != list.end(); it++)
        (*it)->Generate(assembly);
}

SpecifierKind DeclarationSpecifierNode::Kind()
{
    return kind;
}

SymType *ExprNode::GetType() const
{
    return type;
}

void ExprNode::SetType(SymType *type)
{
    this->type = type;
}

void ExprNode::SetValueCategory(ValueCategory category)
{
    this->category = category;
}

ValueCategory ExprNode::GetValueCategory() const
{
    return category;
}

void ExprNode::SetPosition(int row, int col)
{
    position = Position(row, col);
}

void ExprNode::SetPosition(Position position)
{
    this->position = position;
}

Position ExprNode::GetPosition() const
{
    return position;
}

void ExprNode::SetPosition(std::shared_ptr<Token> token)
{
    position = token->position;
}

void IterationStatementNode::SetBody(StatementNode *body)
{
    this->body = body;
}

TypedefIdentifierNode::TypedefIdentifierNode(SymAlias *alias): alias(alias)
{
    this->kind = SpecifierKind::TYPEDEF;
}

std::shared_ptr<Token> TypedefIdentifierNode::GetToken()
{
    return id;
}

SymAlias *TypedefIdentifierNode::GetAlias() const
{
    return alias;
}

void TypedefIdentifierNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "Typedef " << std::endl;
    indent.append(isTail ? "    " : "│   ");
    alias->Print(os, indent, true);
}

void TypedefIdentifierNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}

SimpleInitializer::SimpleInitializer(ExprNode *expr): value(expr) {}

void SimpleInitializer::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "Initializer " << std::endl;
    indent.append(isTail ? "    " : "│   ");
    value->Print(os, indent, true);
}

void SimpleInitializer::SetValue(ExprNode *value)
{
    this->value = value;
}

ExprNode *SimpleInitializer::GetValue() const
{
    return value;
}

void SimpleInitializer::Generate(Asm::Assembly *assembly)
{
    // TODO
}

void DesignatorNode::SetValue(InitializerNode *value)
{
    this->value = value;
}

PrintfNode::PrintfNode(StringLiteralNode *format, ArgumentExprListNode *arguments):
        format(format), arguments(arguments) {}

void PrintfNode::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << "printf " << std::endl;
    indent.append(isTail ? "    " : "│   ");
    format->Print(os, indent, !arguments);
    if (arguments) arguments->Print(os, indent, true);
}

ExprNode *PrintfNode::Eval(Evaluator *evaluator)
{
    return nullptr;
}

void PrintfNode::Generate(Asm::Assembly *assembly)
{
    // TODO
}
