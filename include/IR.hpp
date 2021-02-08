#ifndef SMPLC_IR_DEF
#define SMPLC_IR_DEF

#include <variant>
#include <vector>
#include <memory>
#include <cstdint>

namespace IR{

using index_t = unsigned long long int;
using const_t = int32_t;

using Operand = std::variant<index_t, const_t>;

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

struct InstrBase{
    Operand index;
};


template<Operation op, typename... T>
struct Instr: public InstrBase{
    static const Operation operation = op;
};

template<Operation op, typename T>
struct Instr<op, T>: public InstrBase{
    static const Operation operation = op;
    T operand;
};

template<Operation op, typename T1, typename T2>
struct Instr<op, T1, T2>: public InstrBase{
    static const Operation operation = op;
    T1 operand1;
    T2 operand2;
};

using Neg = Instr<Operation::Neg, Operand>;
using Add = Instr<Operation::Add, Operand, Operand>;
using Sub = Instr<Operation::Sub, Operand, Operand>;
using Mul = Instr<Operation::Mul, Operand, Operand>;
using Div = Instr<Operation::Div, Operand, Operand>;
using Cmp = Instr<Operation::Cmp, Operand, Operand>;
using Adda = Instr<Operation::Adda, Operand, Operand>;
using Load = Instr<Operation::Load, Operand>;
using Store = Instr<Operation::Store, Operand, Operand>;
using Phi = Instr<Operation::Phi, Operand, Operand>;
using End = Instr<Operation::End>;
using Bra = Instr<Operation::Bra, Operand>;
using Bne = Instr<Operation::Bne, Operand, Operand>;
using Beq = Instr<Operation::Beq, Operand, Operand>;
using Ble = Instr<Operation::Ble, Operand, Operand>;
using Blt = Instr<Operation::Blt, Operand, Operand>;
using Bge = Instr<Operation::Bge, Operand, Operand>;
using Bgt = Instr<Operation::Bgt, Operand, Operand>;
using Read = Instr<Operation::Read>;
using Write = Instr<Operation::Write, Operand>;
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

struct BasicBlock{
    std::vector<Instrction> instructions;
    std::shared_ptr<BasicBlock> direct, fall;
};


};

#endif