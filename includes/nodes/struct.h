//
// Created by keltar on 11/4/17.
//

#ifndef TINY_C_COMPILER_STRUCT_H
#define TINY_C_COMPILER_STRUCT_H

#include "node.h"
#include "decls.h"

class StructDeclaratorNode: public DeclaratorNode
{
public:
    StructDeclaratorNode(DeclaratorNode *declarator, ExprNode *constantExpr);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    void Generate(Asm::Assembly *assembly) override;
private:
    ExprNode *constantExpr;
};

class StructDeclaratorListNode: public Node
{
public:
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    void Add(StructDeclaratorNode *initDeclarator);
    uint64_t Size();
    std::list<StructDeclaratorNode *> &List();
    void Generate(Asm::Assembly *assembly) override;
protected:
    std::list<StructDeclaratorNode *> list;
};

class StructDeclarationNode: public Node
{
public:
    explicit StructDeclarationNode(StructDeclaratorListNode *structDeclaratorList):
            structDeclaratorList(structDeclaratorList) {}
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    std::list<StructDeclaratorNode *>  &List() const;
    void Generate(Asm::Assembly *assembly) override;
private:
    StructDeclaratorListNode *structDeclaratorList;
};

class StructDeclarationListNode: public Node
{
public:
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    void Add(StructDeclarationNode *initDeclarator);
    uint64_t Size();
    std::list<StructDeclarationNode *> &List();
    void Generate(Asm::Assembly *assembly) override;
protected:
    std::list<StructDeclarationNode *> list;
};

class StructSpecifierNode: public DeclarationSpecifierNode
{
public:
    explicit StructSpecifierNode(SymRecord *type, std::shared_ptr<Token> structToken);
    void Print(std::ostream &os, std::string ident, bool isTail) override;
    void SetId(IdNode *id);
    IdNode *GetId() const;
    SymRecord *GetRecordType() const;
    void SetRecordType(SymRecord *type);
    std::shared_ptr<Token> GetToken() const;
    void Generate(Asm::Assembly *assembly) override;
private:
    SymRecord *type{nullptr};
    std::shared_ptr<Token> token;
};

#endif //TINY_C_COMPILER_STRUCT_H
