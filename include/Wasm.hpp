#ifndef SMPLC_WASM_DEF
#define SMPLC_WASM_DEF

#include <list>
#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <utility>

using float32_t = float;
using float64_t = double;

namespace Wasm {

enum class ValueType {i32, i64, f32, f64};
enum class DescType {Func, Table, Memory, Global};

using Value = std::variant<int32_t, int64_t, float32_t, float64_t>;

struct FuncType {
    std::vector<ValueType> params;
    std::vector<ValueType> results;
};

struct ConstExpr{
    enum class Type {Value, GlobalIndex} type;
    Value value;
};

struct Ref {
    enum class Type {Func, Extern} type;
    std::optional<uint32_t> addr;
};

using Limit = std::pair<uint32_t, std::optional<uint32_t>>;

template<uint16_t OP>
struct InstrBase{
    static const uint16_t opcode = OP;
};

template<uint16_t OP>
struct BlockInstr : public InstrBase<OP>{
    std::variant<std::monostate, uint32_t, ValueType> blocktype;
};

template<uint16_t OP>
struct UnaryInstr : public InstrBase<OP>{
    uint32_t index;
};

template<uint16_t OP>
struct MemoryInstr : public InstrBase<OP>{
    uint32_t align, offset;
};

using Instr_unreachable = InstrBase<0x00>;
using Instr_nop = InstrBase<0x01>;
using Instr_block = BlockInstr<0x02>;
using Instr_loop = BlockInstr<0x03>;
using Instr_if = BlockInstr<0x04>;
using Instr_else = InstrBase<0x05>;
using Instr_end = InstrBase<0x0B>;
using Instr_br = UnaryInstr<0x0C>;
using Instr_br_if = UnaryInstr<0x0D>;
struct Instr_br_table: public InstrBase<0x0E>{
    std::vector<uint32_t> labelidx;
};
using Instr_return = InstrBase<0x0F>;
using Instr_call = UnaryInstr<0x10>;
struct Instr_call_indirect: public InstrBase<0x11>{
    uint32_t tableidx, typeidx;
};
using Instr_drop = InstrBase<0x1A>;
using Instr_select = InstrBase<0x1B>;
struct Instr_select_t: public InstrBase<0x1C>{
    std::vector<ValueType> valtype;
};
using Instr_local_get = UnaryInstr<0x20>;
using Instr_local_set = UnaryInstr<0x21>;
using Instr_local_tee = UnaryInstr<0x22>;
using Instr_global_get = UnaryInstr<0x23>;
using Instr_global_set = UnaryInstr<0x24>;
using Instr_table_get = UnaryInstr<0x25>;
using Instr_table_set = UnaryInstr<0x26>;

using Instr_i32_load = MemoryInstr<0x28>;
using Instr_i64_load = MemoryInstr<0x29>;
using Instr_f32_load = MemoryInstr<0x2A>;
using Instr_f64_load = MemoryInstr<0x2B>;
using Instr_i32_load8_s = MemoryInstr<0x2C>;
using Instr_i32_load8_u = MemoryInstr<0x2D>;
using Instr_i32_load16_s = MemoryInstr<0x2E>;
using Instr_i32_load16_u = MemoryInstr<0x2F>;
using Instr_i64_load8_s = MemoryInstr<0x30>;
using Instr_i64_load8_u = MemoryInstr<0x31>;
using Instr_i64_load16_s = MemoryInstr<0x32>;
using Instr_i64_load16_u = MemoryInstr<0x33>;
using Instr_i64_load32_s = MemoryInstr<0x34>;
using Instr_i64_load32_u = MemoryInstr<0x35>;
using Instr_i32_store = MemoryInstr<0x36>;
using Instr_i64_store = MemoryInstr<0x37>;
using Instr_f32_store = MemoryInstr<0x38>;
using Instr_f64_store = MemoryInstr<0x39>;
using Instr_i32_store8 = MemoryInstr<0x3A>;
using Instr_i32_store16 = MemoryInstr<0x3B>;
using Instr_i64_store8 = MemoryInstr<0x3C>;
using Instr_i64_store16 = MemoryInstr<0x3D>;
using Instr_i64_store32 = MemoryInstr<0x3E>;
using Instr_memory_size = InstrBase<0x3F>;
using Instr_memory_grow = InstrBase<0x40>;

struct Instr_i32_const: public InstrBase<0x41>{
    int32_t value;
};
struct Instr_i64_const: public InstrBase<0x42>{
    int64_t value;
};
struct Instr_f32_const: public InstrBase<0x43>{
    float32_t value;
};
struct Instr_f64_const: public InstrBase<0x44>{
    float64_t value;
};

using Instr_i32_eqz = InstrBase<0x45>;
using Instr_i32_eq = InstrBase<0x46>;
using Instr_i32_ne = InstrBase<0x47>;
using Instr_i32_lt_s = InstrBase<0x48>;
using Instr_i32_lt_u = InstrBase<0x49>;
using Instr_i32_gt_s = InstrBase<0x4A>;
using Instr_i32_gt_u = InstrBase<0x4B>;
using Instr_i32_le_s = InstrBase<0x4C>;
using Instr_i32_le_u = InstrBase<0x4D>;
using Instr_i32_ge_s = InstrBase<0x4E>;
using Instr_i32_ge_u = InstrBase<0x4F>;
using Instr_i64_eqz = InstrBase<0x50>;
using Instr_i64_eq = InstrBase<0x51>;
using Instr_i64_ne = InstrBase<0x52>;
using Instr_i64_lt_s = InstrBase<0x53>;
using Instr_i64_lt_u = InstrBase<0x54>;
using Instr_i64_gt_s = InstrBase<0x55>;
using Instr_i64_gt_u = InstrBase<0x56>;
using Instr_i64_le_s = InstrBase<0x57>;
using Instr_i64_le_u = InstrBase<0x58>;
using Instr_i64_ge_s = InstrBase<0x59>;
using Instr_i64_ge_u = InstrBase<0x5A>;
using Instr_f32_eq = InstrBase<0x5B>;
using Instr_f32_ne = InstrBase<0x5C>;
using Instr_f32_lt = InstrBase<0x5D>;
using Instr_f32_gt = InstrBase<0x5E>;
using Instr_f32_le = InstrBase<0x5F>;
using Instr_f32_ge = InstrBase<0x60>;
using Instr_f64_eq = InstrBase<0x61>;
using Instr_f64_ne = InstrBase<0x62>;
using Instr_f64_lt = InstrBase<0x63>;
using Instr_f64_gt = InstrBase<0x64>;
using Instr_f64_le = InstrBase<0x65>;
using Instr_f64_ge = InstrBase<0x66>;
using Instr_i32_clz = InstrBase<0x67>;
using Instr_i32_ctz = InstrBase<0x68>;
using Instr_i32_popcnt = InstrBase<0x69>;
using Instr_i32_add = InstrBase<0x6A>;
using Instr_i32_sub = InstrBase<0x6B>;
using Instr_i32_mul = InstrBase<0x6C>;
using Instr_i32_div_s = InstrBase<0x6D>;
using Instr_i32_div_u = InstrBase<0x6E>;
using Instr_i32_rem_s = InstrBase<0x6F>;
using Instr_i32_rem_u = InstrBase<0x70>;
using Instr_i32_and = InstrBase<0x71>;
using Instr_i32_or = InstrBase<0x72>;
using Instr_i32_xor = InstrBase<0x73>;
using Instr_i32_shl = InstrBase<0x74>;
using Instr_i32_shr_s = InstrBase<0x75>;
using Instr_i32_shr_u = InstrBase<0x76>;
using Instr_i32_rotl = InstrBase<0x77>;
using Instr_i32_rotr = InstrBase<0x78>;
using Instr_i64_clz = InstrBase<0x79>;
using Instr_i64_ctz = InstrBase<0x7A>;
using Instr_i64_popcnt = InstrBase<0x7B>;
using Instr_i64_add = InstrBase<0x7C>;
using Instr_i64_sub = InstrBase<0x7D>;
using Instr_i64_mul = InstrBase<0x7E>;
using Instr_i64_div_s = InstrBase<0x7F>;
using Instr_i64_div_u = InstrBase<0x80>;
using Instr_i64_rem_s = InstrBase<0x81>;
using Instr_i64_rem_u = InstrBase<0x82>;
using Instr_i64_and = InstrBase<0x83>;
using Instr_i64_or = InstrBase<0x84>;
using Instr_i64_xor = InstrBase<0x85>;
using Instr_i64_shl = InstrBase<0x86>;
using Instr_i64_shr_s = InstrBase<0x87>;
using Instr_i64_shr_u = InstrBase<0x88>;
using Instr_i64_rotl = InstrBase<0x89>;
using Instr_i64_rotr = InstrBase<0x8A>;
using Instr_f32_abs = InstrBase<0x8B>;
using Instr_f32_neg = InstrBase<0x8C>;
using Instr_f32_ceil = InstrBase<0x8D>;
using Instr_f32_floor = InstrBase<0x8E>;
using Instr_f32_trunc = InstrBase<0x8F>;
using Instr_f32_nearest = InstrBase<0x90>;
using Instr_f32_sqrt = InstrBase<0x91>;
using Instr_f32_add = InstrBase<0x92>;
using Instr_f32_sub = InstrBase<0x93>;
using Instr_f32_mul = InstrBase<0x94>;
using Instr_f32_div = InstrBase<0x95>;
using Instr_f32_min = InstrBase<0x96>;
using Instr_f32_max = InstrBase<0x97>;
using Instr_f32_copysign = InstrBase<0x98>;
using Instr_f64_abs = InstrBase<0x99>;
using Instr_f64_neg = InstrBase<0x9A>;
using Instr_f64_ceil = InstrBase<0x9B>;
using Instr_f64_floor = InstrBase<0x9C>;
using Instr_f64_trunc = InstrBase<0x9D>;
using Instr_f64_nearest = InstrBase<0x9E>;
using Instr_f64_sqrt = InstrBase<0x9F>;
using Instr_f64_add = InstrBase<0xA0>;
using Instr_f64_sub = InstrBase<0xA1>;
using Instr_f64_mul = InstrBase<0xA2>;
using Instr_f64_div = InstrBase<0xA3>;
using Instr_f64_min = InstrBase<0xA4>;
using Instr_f64_max = InstrBase<0xA5>;
using Instr_f64_copysign = InstrBase<0xA6>;
using Instr_i32_wrap_i64 = InstrBase<0xA7>;
using Instr_i32_trunc_s_f32 = InstrBase<0xA8>;
using Instr_i32_trunc_u_f32 = InstrBase<0xA9>;
using Instr_i32_trunc_s_f64 = InstrBase<0xAA>;
using Instr_i32_trunc_u_f64 = InstrBase<0xAB>;
using Instr_i64_extend_s_i32 = InstrBase<0xAC>;
using Instr_i64_extend_u_i32 = InstrBase<0xAD>;
using Instr_i64_trunc_s_f32 = InstrBase<0xAE>;
using Instr_i64_trunc_u_f32 = InstrBase<0xAF>;
using Instr_i64_trunc_s_f64 = InstrBase<0xB0>;
using Instr_i64_trunc_u_f64 = InstrBase<0xB1>;
using Instr_f32_convert_s_i32 = InstrBase<0xB2>;
using Instr_f32_convert_u_i32 = InstrBase<0xB3>;
using Instr_f32_convert_s_i64 = InstrBase<0xB4>;
using Instr_f32_convert_u_i64 = InstrBase<0xB5>;
using Instr_f32_demote_f64 = InstrBase<0xB6>;
using Instr_f64_convert_s_i32 = InstrBase<0xB7>;
using Instr_f64_convert_u_i32 = InstrBase<0xB8>;
using Instr_f64_convert_s_i64 = InstrBase<0xB9>;
using Instr_f64_convert_u_i64 = InstrBase<0xBA>;
using Instr_f64_promote_f32 = InstrBase<0xBB>;
using Instr_i32_reinterpret_f32 = InstrBase<0xBC>;
using Instr_i64_reinterpret_f64 = InstrBase<0xBD>;
using Instr_f32_reinterpret_i32 = InstrBase<0xBE>;
using Instr_f64_reinterpret_i64 = InstrBase<0xBF>;
using Instr_i32_extend8_s = InstrBase<0xC0>;
using Instr_i32_extend16_s = InstrBase<0xC1>;
using Instr_i64_extend8_s = InstrBase<0xC2>;
using Instr_i64_extend16_s = InstrBase<0xC3>;
using Instr_i64_extend32_s = InstrBase<0xC4>;

struct Instr_ref_null: public InstrBase<0xD0>{
    Ref::Type reftype;
};
using Instr_ref_is_null = InstrBase<0xD1>;
using Instr_ref_func = UnaryInstr<0xD2>;

using Instr_i32_trunc_sat_f32_s = InstrBase<0xFC00>;
using Instr_i32_trunc_sat_f32_u = InstrBase<0xFC01>;
using Instr_i32_trunc_sat_f64_s = InstrBase<0xFC02>;
using Instr_i32_trunc_sat_f64_u = InstrBase<0xFC03>;
using Instr_i64_trunc_sat_f32_s = InstrBase<0xFC04>;
using Instr_i64_trunc_sat_f32_u = InstrBase<0xFC05>;
using Instr_i64_trunc_sat_f64_s = InstrBase<0xFC06>;
using Instr_i64_trunc_sat_f64_u = InstrBase<0xFC07>;

using Instr_memory_init = UnaryInstr<0xFC08>;
using Instr_data_drop = UnaryInstr<0xFC09>;
using Instr_memory_copy = InstrBase<0xFC0A>;
using Instr_memory_fill = InstrBase<0xFC0B>;
struct Instr_table_init: public InstrBase<0xFC0C>{
    uint32_t tableidx, elemidx;
};
using Instr_elem_drop = UnaryInstr<0xFC0D>;
struct Instr_table_copy: public InstrBase<0xFC0E>{
    uint32_t table1, table2;
};
using Instr_table_grow = UnaryInstr<0xFC0F>;
using Instr_table_size = UnaryInstr<0xFC10>;
using Instr_table_fill = UnaryInstr<0xFC11>;

using Instr = std::variant<
    Instr_unreachable,
    Instr_nop,
    Instr_block,
    Instr_loop,
    Instr_if,
    Instr_else,
    Instr_end,
    Instr_br,
    Instr_br_if,
    Instr_br_table,
    Instr_return,
    Instr_call,
    Instr_call_indirect,
    Instr_drop,
    Instr_select,
    Instr_select_t,
    Instr_local_get,
    Instr_local_set,
    Instr_local_tee,
    Instr_global_get,
    Instr_global_set,
    Instr_table_get,
    Instr_table_set,
    Instr_i32_load,
    Instr_i64_load,
    Instr_f32_load,
    Instr_f64_load,
    Instr_i32_load8_s,
    Instr_i32_load8_u,
    Instr_i32_load16_s,
    Instr_i32_load16_u,
    Instr_i64_load8_s,
    Instr_i64_load8_u,
    Instr_i64_load16_s,
    Instr_i64_load16_u,
    Instr_i64_load32_s,
    Instr_i64_load32_u,
    Instr_i32_store,
    Instr_i64_store,
    Instr_f32_store,
    Instr_f64_store,
    Instr_i32_store8,
    Instr_i32_store16,
    Instr_i64_store8,
    Instr_i64_store16,
    Instr_i64_store32,
    Instr_memory_size,
    Instr_memory_grow,
    Instr_i32_const,
    Instr_i64_const,
    Instr_f32_const,
    Instr_f64_const,
    Instr_i32_eqz,
    Instr_i32_eq,
    Instr_i32_ne,
    Instr_i32_lt_s,
    Instr_i32_lt_u,
    Instr_i32_gt_s,
    Instr_i32_gt_u,
    Instr_i32_le_s,
    Instr_i32_le_u,
    Instr_i32_ge_s,
    Instr_i32_ge_u,
    Instr_i64_eqz,
    Instr_i64_eq,
    Instr_i64_ne,
    Instr_i64_lt_s,
    Instr_i64_lt_u,
    Instr_i64_gt_s,
    Instr_i64_gt_u,
    Instr_i64_le_s,
    Instr_i64_le_u,
    Instr_i64_ge_s,
    Instr_i64_ge_u,
    Instr_f32_eq,
    Instr_f32_ne,
    Instr_f32_lt,
    Instr_f32_gt,
    Instr_f32_le,
    Instr_f32_ge,
    Instr_f64_eq,
    Instr_f64_ne,
    Instr_f64_lt,
    Instr_f64_gt,
    Instr_f64_le,
    Instr_f64_ge,
    Instr_i32_clz,
    Instr_i32_ctz,
    Instr_i32_popcnt,
    Instr_i32_add,
    Instr_i32_sub,
    Instr_i32_mul,
    Instr_i32_div_s,
    Instr_i32_div_u,
    Instr_i32_rem_s,
    Instr_i32_rem_u,
    Instr_i32_and,
    Instr_i32_or,
    Instr_i32_xor,
    Instr_i32_shl,
    Instr_i32_shr_s,
    Instr_i32_shr_u,
    Instr_i32_rotl,
    Instr_i32_rotr,
    Instr_i64_clz,
    Instr_i64_ctz,
    Instr_i64_popcnt,
    Instr_i64_add,
    Instr_i64_sub,
    Instr_i64_mul,
    Instr_i64_div_s,
    Instr_i64_div_u,
    Instr_i64_rem_s,
    Instr_i64_rem_u,
    Instr_i64_and,
    Instr_i64_or,
    Instr_i64_xor,
    Instr_i64_shl,
    Instr_i64_shr_s,
    Instr_i64_shr_u,
    Instr_i64_rotl,
    Instr_i64_rotr,
    Instr_f32_abs,
    Instr_f32_neg,
    Instr_f32_ceil,
    Instr_f32_floor,
    Instr_f32_trunc,
    Instr_f32_nearest,
    Instr_f32_sqrt,
    Instr_f32_add,
    Instr_f32_sub,
    Instr_f32_mul,
    Instr_f32_div,
    Instr_f32_min,
    Instr_f32_max,
    Instr_f32_copysign,
    Instr_f64_abs,
    Instr_f64_neg,
    Instr_f64_ceil,
    Instr_f64_floor,
    Instr_f64_trunc,
    Instr_f64_nearest,
    Instr_f64_sqrt,
    Instr_f64_add,
    Instr_f64_sub,
    Instr_f64_mul,
    Instr_f64_div,
    Instr_f64_min,
    Instr_f64_max,
    Instr_f64_copysign,
    Instr_i32_wrap_i64,
    Instr_i32_trunc_s_f32,
    Instr_i32_trunc_u_f32,
    Instr_i32_trunc_s_f64,
    Instr_i32_trunc_u_f64,
    Instr_i64_extend_s_i32,
    Instr_i64_extend_u_i32,
    Instr_i64_trunc_s_f32,
    Instr_i64_trunc_u_f32,
    Instr_i64_trunc_s_f64,
    Instr_i64_trunc_u_f64,
    Instr_f32_convert_s_i32,
    Instr_f32_convert_u_i32,
    Instr_f32_convert_s_i64,
    Instr_f32_convert_u_i64,
    Instr_f32_demote_f64,
    Instr_f64_convert_s_i32,
    Instr_f64_convert_u_i32,
    Instr_f64_convert_s_i64,
    Instr_f64_convert_u_i64,
    Instr_f64_promote_f32,
    Instr_i32_reinterpret_f32,
    Instr_i64_reinterpret_f64,
    Instr_f32_reinterpret_i32,
    Instr_f64_reinterpret_i64,
    Instr_i32_extend8_s,
    Instr_i32_extend16_s,
    Instr_i64_extend8_s,
    Instr_i64_extend16_s,
    Instr_i64_extend32_s,
    Instr_ref_null,
    Instr_ref_is_null,
    Instr_ref_func,
    Instr_i32_trunc_sat_f32_s,
    Instr_i32_trunc_sat_f32_u,
    Instr_i32_trunc_sat_f64_s,
    Instr_i32_trunc_sat_f64_u,
    Instr_i64_trunc_sat_f32_s,
    Instr_i64_trunc_sat_f32_u,
    Instr_i64_trunc_sat_f64_s,
    Instr_i64_trunc_sat_f64_u,
    Instr_memory_init,
    Instr_data_drop,
    Instr_memory_copy,
    Instr_memory_fill,
    Instr_table_init,
    Instr_elem_drop,
    Instr_table_copy,
    Instr_table_grow,
    Instr_table_fill,
    Instr_table_size
>;

using Memory = Limit;

struct Table {
    Limit limits;
    Ref::Type refType;
};

struct GlobalType{
    bool mut;
    ValueType valueType;
};

struct Import {
    std::string module;
    std::string name;
    std::variant<
        uint32_t,
        Table,
        Memory,
        GlobalType
    > desc;
};

struct Func {
    uint32_t type;
    std::vector<ValueType> locals;
    std::list<Instr> body;
};

struct Global: public GlobalType {
    ConstExpr  init;
};

struct Export {
    std::string name;
    DescType    descType;
    uint32_t    descIdx;
};

struct PassiveMode {};
struct DeclarativeMode {};
struct ActiveMode {
    uint32_t index;
    ConstExpr offset;
};

struct Elem {
    Ref::Type type;
    std::vector<ConstExpr> init;
    std::variant<PassiveMode, ActiveMode, DeclarativeMode> mode;
};

struct Data {
    std::string init;
    std::variant<PassiveMode, ActiveMode, DeclarativeMode> mode;
};

struct Module{
    std::vector<FuncType>   types;
    std::vector<Import>     imports;
    std::vector<Func>       funcs;
    std::vector<Table>      tables;
    std::vector<Memory>     mems;
    std::vector<Global>     globals;
    std::vector<Export>     exports;
    std::optional<uint32_t> start;
    std::vector<Elem>   elems;
    std::vector<Data>   datas;
};

}
#endif