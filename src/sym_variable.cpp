#include "../includes/symbols/sym_variable.h"
#include "../includes/symbols/sym_type.h"

SymVariable::SymVariable(std::string name, SymType *type, IdNode *id): type(type), id(id)
{
    this->name = std::move(name);
    symbolClass = SymbolClass::VARIABLE;
}

void SymVariable::SetType(SymType *type)
{
    this->type = type;
}

SymType *SymVariable::GetType() const
{
    return type;
}

void SymVariable::Print(std::ostream &os, std::string indent, bool isTail)
{
    os << indent << (isTail ? "└── " : "├── ");
    os << name << std::endl;
    indent.append(isTail ? "    " : "│   ");
    type->Print(os, indent, true);
}

IdNode *SymVariable::GetId() const
{
    return id;
}

void SymVariable::SetId(IdNode *id)
{
    this->id = id;
}

void SymVariable::SetOffset(int32_t offset)
{
    this->offset = offset;
}

int32_t SymVariable::GetOffset() const
{
    return offset;
}
