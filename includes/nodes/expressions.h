//
// Created by keltar on 11/4/17.
//

#ifndef TINY_C_COMPILER_EXPRESSIONS_H
#define TINY_C_COMPILER_EXPRESSIONS_H

#include "node.h"
#include "../token.h"
#include <list>
#include <value_category.h>

class SymType;

class Evaluator;

class ExprNode: public Node
{
public:
    void Print(std::ostream &os, std::string ident, bool isTail) override = 0;
    SymType *GetType() const;
    void SetType(SymType *type);
    ValueCategory  GetValueCategory() const;
    void SetValueCategory(ValueCategory category);
    virtual ExprNode * Eval(Evaluator *evaluator) = 0;
    Position GetPosition() const;
    void SetPosition(Position position);
    void SetPosition(int row, int col);
    void SetPosition(std::shared_ptr<Token> token);
    void Generate(Asm::Assembly *assembly) override = 0;
protected:
    SymType *type{nullptr};
    ValueCategory  category{ValueCategory::RVALUE};
    Position position{-1, -1};
};

class PostfixIncrementNode: public ExprNode
{
public:
    explicit PostfixIncrementNode(ExprNode *node);
    virtual void Print(std::ostream &os, std::string ident, bool isTail);
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *expr;
};

class PostfixDecrementNode: public ExprNode
{
public:
    explicit PostfixDecrementNode(ExprNode *node);
    virtual void Print(std::ostream &os, std::string ident, bool isTail);
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *expr;
};

class IdNode;

class StructureOrUnionMemberAccessNode: public ExprNode
{
public:
    StructureOrUnionMemberAccessNode(ExprNode *structureOrUnion, IdNode *member);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *structureOrUnion;
    IdNode *member;
};

class StructureOrUnionMemberAccessByPointerNode: public ExprNode
{
public:
    StructureOrUnionMemberAccessByPointerNode(ExprNode *structureOrUnion, IdNode *member);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *structureOrUnion;
    IdNode *member;
};

class ArrayAccessNode: public ExprNode
{
public:
    ArrayAccessNode(ExprNode *left, ExprNode *inBrackets);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *array, *index;
};

class ArgumentExprListNode: public Node
{
public:
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    void Add(ExprNode *assignmentExpr);
    uint64_t Size();
    std::list<ExprNode *> &List();
    void Generate(Asm::Assembly *assembly) override;
    int32_t GetArgumentsSize();
private:
    std::list<ExprNode *> list;
};

class FunctionCallNode: public ExprNode
{
public:
    FunctionCallNode(ExprNode *function, ArgumentExprListNode *arguments);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *function;
    ArgumentExprListNode *arguments;
};

class StringLiteralNode;

class PrintfNode: public ExprNode
{
public:
    PrintfNode(StringLiteralNode *format, ArgumentExprListNode *arguments);
    void Print(std::ostream &os, std::string indent, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    StringLiteralNode *format;
    ArgumentExprListNode *arguments;
};

class SizeofExprNode: public  ExprNode
{
public:
    explicit SizeofExprNode(ExprNode *expr);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *expr;
};

class TypeNameNode;

class SizeofTypeNameNode: public ExprNode
{
public:
    explicit SizeofTypeNameNode(SymType *typeName);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    SymType *typeName;
};

class UnaryOpNode: public ExprNode
{
public:
    explicit UnaryOpNode(std::shared_ptr<Token> unaryOp, ExprNode *expr);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    void int32Generate(Asm::Assembly *assembly);
    void floatGenerate(Asm::Assembly *assembly);
    void pointerGenerate(Asm::Assembly *assembly);
    std::shared_ptr<Token> unaryOp;
    ExprNode *expr;
};

class PrefixIncrementNode: public ExprNode
{
public:
    explicit PrefixIncrementNode(ExprNode *node);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *expr;
};

class PrefixDecrementNode: public ExprNode
{
public:
    explicit PrefixDecrementNode(ExprNode *node);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *expr;
};

class BinOpNode: public ExprNode
{
public:
    BinOpNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> op);
    BinOpNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> op, SymType *resultType);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    ExprNode *Left() const;
    ExprNode *Right() const;
    std::shared_ptr<Token> GetOperation() const;
    void Generate(Asm::Assembly *assembly) override;
private:
    void int32Generate(Asm::Assembly *assembly);
    void floatGenerate(Asm::Assembly *assembly);
    void logicalAndOrGenerate(Asm::Assembly *assembly);
    void pointerGenerate(Asm::Assembly *assembly);
    bool isRelop(TokenType tokenType);
    ExprNode *left, *right;
    std::shared_ptr<Token> op;
};

class TernaryOperatorNode: public ExprNode
{
public:
    TernaryOperatorNode(ExprNode *condition, ExprNode *iftrue, ExprNode *iffalse);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *condition, *iftrue, *iffalse;
};

class SymArray;
class SymRecord;

class AssignmentNode: public ExprNode
{
public:
    AssignmentNode(ExprNode *left, ExprNode *right, std::shared_ptr<Token> assignmentOp);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    void assignArray(Asm::Assembly *assembly, int32_t offset, SymArray *array);
    void assignStructure(Asm::Assembly *assembly, int32_t offset, SymRecord *record);
    ExprNode *left, *right;
    std::shared_ptr<Token> assignmentOp;
};

class CommaSeparatedExprs: public ExprNode
{
public:
    CommaSeparatedExprs() = default;
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
    void Add(ExprNode *expr);
    uint64_t Size();
    std::list<ExprNode *> &List();
private:
    std::list<ExprNode *> expressions;
};

class SymVariable;

class IdNode: public ExprNode
{
public:
    explicit IdNode(std::shared_ptr<Token> token);
    explicit IdNode(std::shared_ptr<Token> token, SymType *type);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    std::string GetName() const;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
    void SetVariable(SymVariable *variable);
    SymVariable *GetVariable() const;
private:
    void arrayGenerate(Asm::Assembly *assembly, int32_t offset, SymArray *array);
    void structureGenerate(Asm::Assembly *assembly, int32_t offset, SymRecord *record);
    SymVariable *variable;
    std::shared_ptr<Token> token;
};

class IntConstNode;
class FloatConstNode;

class ConstNode: public ExprNode
{
public:
    void Print(std::ostream &os, std::string ident, bool isTail) override = 0;
    ExprNode *Eval(Evaluator *evaluator) override = 0;
    void Generate(Asm::Assembly *assembly) override = 0;
    static IntConstNode *IntZero();
    static IntConstNode *IntOne();
    static FloatConstNode *FloatZero();
    static FloatConstNode *FloatOne();
protected:
};

class IntConstNode: public ConstNode
{
public:
    explicit IntConstNode(std::shared_ptr<Token> token);
    IntConstNode(int32_t value);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    int32_t GetValue() const;
    void Generate(Asm::Assembly *assembly) override;
private:
    int32_t value;
};

class FloatConstNode: public ConstNode
{
public:
    explicit FloatConstNode(std::shared_ptr<Token> token);
    FloatConstNode(float value);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    float GetValue() const;
    void Generate(Asm::Assembly *assembly) override;
private:
    float value;
};

class StringLiteralNode: public ExprNode
{
public:
    explicit StringLiteralNode(std::shared_ptr<Token> token);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    ExprNode *Eval(Evaluator *evaluator) override;
    void Generate(Asm::Assembly *assembly) override;
    Asm::AsmLabel *GetAddress() const;
private:
    std::shared_ptr<Token> token;
    Asm::AsmLabel *address;
};

#endif //TINY_C_COMPILER_EXPRESSIONS_H
