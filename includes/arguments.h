//
// Created by keltar on 12/17/17.
//

#ifndef TINY_C_COMPILER_ARGUMENTS_H
#define TINY_C_COMPILER_ARGUMENTS_H

#include <ostream>
#include <unordered_map>
#include <iostream>

class ConstNode;

namespace Asm
{
    class Argument
    {
    public:
        virtual void Print(std::ostream &os) = 0;
    };

    enum class Register
    {
        EAX, EBX, ECX, EDX, EBP, ESP
    };

    static std::unordered_map<Register, std::string> RegisterToString =
    {
            {Register::EAX, "eax"}, {Register::EBX, "ebx"},
            {Register::ECX, "ecx"}, {Register::EDX, "edx"},
            {Register::EBP, "ebp"}, {Register::ESP, "esp"}
    };

    class ArgumentRegister: public Argument
    {
    public:
        explicit ArgumentRegister(Register aregister);
        void Print(std::ostream &os) override;
    private:
        Register aregister;
    };

    static std::unordered_map<Register, ArgumentRegister *> Registers =
    {
            {Register::EAX, new ArgumentRegister(Register::EAX)},
            {Register::EBX, new ArgumentRegister(Register::EBX)},
            {Register::ECX, new ArgumentRegister(Register::ECX)},
            {Register::EDX, new ArgumentRegister(Register::EDX)},
            {Register::EBP, new ArgumentRegister(Register::EBP)},
            {Register::ESP, new ArgumentRegister(Register::ESP)},

    };

    class ArgumentAddress: public Argument
    {
    public:
        explicit ArgumentAddress(Argument *base);
//        explicit ArgumentAddress(Argument *&base);
        ArgumentAddress(int32_t address, Argument *base);
        void Print(std::ostream &os) override;
    private:
        int32_t address{0}, multiplier{0};
        Argument *base{nullptr}, *index{nullptr};
    };

    template<typename... Args>
    ArgumentAddress *MakeAddress(Args... args)
    {
        return new ArgumentAddress(args...);
    }

    class ArgumentConstant: public Argument
    {
    public:
        explicit ArgumentConstant(ConstNode *constNode);
        void Print(std::ostream &os) override;
    private:
        ConstNode *constant;
    };

    std::ostream &operator<<(std::ostream &os, Argument *argument);
}



#endif //TINY_C_COMPILER_ARGUMENTS_H
