#include "../includes/symbols/sym_variable.h"

SymVariable::SymVariable(std::string name, SymType *type): type(type)
{
    this->name = std::move(name);
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