#ifndef SMPLC_IR_DEF
#define SMPLC_IR_DEF

#include <variant>

namespace IR{

using operand_t = unsigned long long int;

enum class Operation{
    Neg,
    Add,
    Sub,
    Mul,
    Div,
    Cmp,
    Adda,
    Load,
    Store,
    Phi,
    End,
    Bra,
    Bne,
    Beq,
    Ble,
    Blt,
    Bge,
    Bgt,
    Read,
    Write,
    WriteNL,
};

template<Operation op, typename... T>
struct Instr{
    static const Operation operation = op;
};

template<Operation op, typename T>
struct Instr<op, T>{
    static const Operation operation = op;
    T operand;
};

template<Operation op, typename T1, typename T2>
struct Instr<op, T1, T2>{
    static const Operation operation = op;
    T1 operand1;
    T2 operand2;
};

using Neg = Instr<Operation::Neg, operand_t>;
using Add = Instr<Operation::Add, operand_t, operand_t>;
using Sub = Instr<Operation::Sub, operand_t, operand_t>;
using Mul = Instr<Operation::Mul, operand_t, operand_t>;
using Div = Instr<Operation::Div, operand_t, operand_t>;
using Cmp = Instr<Operation::Cmp, operand_t, operand_t>;
using Adda = Instr<Operation::Adda, operand_t, operand_t>;
using Load = Instr<Operation::Load, operand_t>;
using Store = Instr<Operation::Store, operand_t, operand_t>;
using Phi = Instr<Operation::Phi, operand_t, operand_t>;
using End = Instr<Operation::End>;
using Bra = Instr<Operation::Bra, operand_t>;
using Bne = Instr<Operation::Bne, operand_t, operand_t>;
using Beq = Instr<Operation::Beq, operand_t, operand_t>;
using Ble = Instr<Operation::Ble, operand_t, operand_t>;
using Blt = Instr<Operation::Blt, operand_t, operand_t>;
using Bge = Instr<Operation::Bge, operand_t, operand_t>;
using Bgt = Instr<Operation::Bgt, operand_t, operand_t>;
using Read = Instr<Operation::Read>;
using Write = Instr<Operation::Write, operand_t>;
using WriteNL = Instr<Operation::WriteNL>;

using Instrction = std::variant<
    Neg,
    Add,
    Sub,
    Mul,
    Div,
    Cmp,
    Adda,
    Load,
    Store,
    Phi,
    End,
    Bra,
    Bne,
    Beq,
    Ble,
    Blt,
    Bge,
    Bgt,
    Read,
    Write,
    WriteNL
>;

};

#endif