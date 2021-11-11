#ifndef SYLVANITE_INCLUDE_AST_H_
#define SYLVANITE_INCLUDE_AST_H_

#include <limits>

using AstIndex = size_t;
using StatePayload = char;
using CellValue = StatePayload;
using ShiftDist = StatePayload;
constexpr StatePayload kMaxStatePayload = std::numeric_limits<StatePayload>::max();
constexpr StatePayload kMinStatePayload = std::numeric_limits<StatePayload>::min();
constexpr StatePayload kMaxCellValue = std::numeric_limits<CellValue>::max();
constexpr StatePayload kMinCellValue = std::numeric_limits<CellValue>::min();
constexpr StatePayload kMaxShiftDist = std::numeric_limits<ShiftDist>::max();
constexpr StatePayload kMinShiftDist = std::numeric_limits<ShiftDist>::min();

//constexpr int kAtomicStatePayloadWidth = 8;//24; TODO

enum AtomicStateTag : uint8_t {
    Eof,        // EOF
    LoopBegin,  // (\[)
    LoopEnd,    // (\])
    CellModify, // ([\+\-]+)
    CellWrite,  // (,)
    CellPrint,  // (\.)
    Shift,      // ([><]+)
};

enum MachineStateTag : uint8_t {
    Null,
    BranchForward,
    BranchBackward,
    Action,
};

struct AtomicState {
    // PAYLOAD is only active when TAG equals 'CellModify' or 'Shift', otherwise 0
    // TODO fix wrapping behavior
    // int32_t payload : kAtomicStatePayloadWidth;
    // AtomicStateTag tag : 8;
    StatePayload payload;
    AtomicStateTag tag;

};

struct BranchOffset {
    int32_t offset : 24;
} __attribute__((packed));

struct ActionState {
    ShiftDist shift_distance;
    CellValue cell_addend;
    uint8_t print : 1;
    uint8_t read : 1;
} __attribute__((packed));



union MachineStatePayload {
    ActionState action;
    BranchOffset offset;
};

struct MachineState {
    MachineStatePayload payload;
    MachineStateTag tag;

} __attribute__((packed));

#endif // SYLVANITE_INCLUDE_AST_H_
