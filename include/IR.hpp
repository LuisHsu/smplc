#ifndef SMPLC_IR_DEF
#define SMPLC_IR_DEF

#include <variant>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

namespace IR{

using index_t = unsigned long long int;

enum class Operation{
    Nop,
    Const,
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
    index_t index;
    InstrBase();
};


template<Operation op, typename... T>
struct Instr: public InstrBase{
    static const Operation operation = op;
};

template<Operation op, typename T>
struct Instr<op, T>: public InstrBase{
    static const Operation operation = op;
    T operand;
    
    Instr(T oper):operand(oper){};
};

template<Operation op, typename T1, typename T2>
struct Instr<op, T1, T2>: public InstrBase{
    static const Operation operation = op;
    T1 operand1;
    T2 operand2;
    
    Instr(T1 oper1, T2 oper2): operand1(oper1), operand2(oper2){};
};

struct Const: public InstrBase{
    static const Operation operation = Operation::Const;
    int32_t value;
    
    Const(int32_t val):value(val){};
};

using Nop = Instr<Operation::Nop>;
using End = Instr<Operation::End>;
using Read = Instr<Operation::Read>;
using WriteNL = Instr<Operation::WriteNL>;
using Neg = Instr<Operation::Neg, index_t>;
using Load = Instr<Operation::Load, index_t>;
using Bra = Instr<Operation::Bra, index_t>;
using Write = Instr<Operation::Write, index_t>;
using Add = Instr<Operation::Add, index_t, index_t>;
using Sub = Instr<Operation::Sub, index_t, index_t>;
using Mul = Instr<Operation::Mul, index_t, index_t>;
using Div = Instr<Operation::Div, index_t, index_t>;
using Cmp = Instr<Operation::Cmp, index_t, index_t>;
using Adda = Instr<Operation::Adda, index_t, index_t>;
using Store = Instr<Operation::Store, index_t, index_t>;
using Phi = Instr<Operation::Phi, index_t, index_t>;
using Bne = Instr<Operation::Bne, index_t, index_t>;
using Beq = Instr<Operation::Beq, index_t, index_t>;
using Ble = Instr<Operation::Ble, index_t, index_t>;
using Blt = Instr<Operation::Blt, index_t, index_t>;
using Bge = Instr<Operation::Bge, index_t, index_t>;
using Bgt = Instr<Operation::Bgt, index_t, index_t>;

using Instrction = std::variant<
    Nop,
    Const,
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

const index_t getInstrIndex(const Instrction&);

struct BasicBlock{
    std::vector<Instrction> instructions;
    std::shared_ptr<BasicBlock> branch, fallThrough;
    std::unordered_map<std::string, IR::index_t> variableVal;
};

};

#endif