#ifndef APP_SCRIPT_SCRIPT_H
#define APP_SCRIPT_SCRIPT_H

#include "pokemon/types.h"

typedef struct ScriptState ScriptState;
typedef s32 (*ScriptTaskCallback)(ScriptState *, s32);
typedef s32 (*ScriptOpcode)(ScriptState *);

typedef enum ScriptValueType {
    SCRIPT_VALUE_NONE,
    SCRIPT_VALUE_FLOAT,
    SCRIPT_VALUE_INT,
    SCRIPT_VALUE_COUNT,
} ScriptValueType;

typedef enum ScriptOperandFlag {
    SCRIPT_OPERAND_VARIABLE = 0x20,
    SCRIPT_OPERAND_LOCAL = 0x40,
    SCRIPT_OPERAND_INDIRECT = 0x80,
    SCRIPT_OPERAND_ADDRESS = 0x100,
} ScriptOperandFlag;

#define SCRIPT_VALUE_MASK 0x3F
#define SCRIPT_STACK_SIZE 64
#define SCRIPT_STD_MAX_ARGS 8

typedef enum ScriptStdArgType {
    SCRIPT_STD_ARG_END,
    SCRIPT_STD_ARG_INT,
    SCRIPT_STD_ARG_FLOAT,
} ScriptStdArgType;

typedef u8 ScriptStateCode;

typedef enum ScriptStateId {
    SCRIPT_STATE_STANDBY,
    SCRIPT_STATE_RESUMED,
    SCRIPT_STATE_YIELDING,
    SCRIPT_STATE_TERMINATE,
    SCRIPT_STATE_TERMINATE_REQUEST,
} ScriptStateId;

typedef enum ScriptOpcodeId {
    SCRIPT_OPCODE_NOP,
    SCRIPT_OPCODE_END,
    SCRIPT_OPCODE_PUSH_MINUS_ONE,
    SCRIPT_OPCODE_POP,
    SCRIPT_OPCODE_CALL,
    SCRIPT_OPCODE_RETURN,
    SCRIPT_OPCODE_JUMP,
    SCRIPT_OPCODE_JUMP_FALSE,
    SCRIPT_OPCODE_PUSH_WORD,
    SCRIPT_OPCODE_UNK_9,
    SCRIPT_OPCODE_UNK_10,
    SCRIPT_OPCODE_UNK_11,
    SCRIPT_OPCODE_UNK_12,
    SCRIPT_OPCODE_BITWISE_AND,
    SCRIPT_OPCODE_BITWISE_OR,
    SCRIPT_OPCODE_PUSH_VAR,
    SCRIPT_OPCODE_PUSH_MULTI_VAR,
    SCRIPT_OPCODE_LOGICAL_NOT,
    SCRIPT_OPCODE_NEGATE,
    SCRIPT_OPCODE_PUSH_VAR_2,
    SCRIPT_OPCODE_MULTIPLY,
    SCRIPT_OPCODE_DIVIDE,
    SCRIPT_OPCODE_MODULO,
    SCRIPT_OPCODE_ADD,
    SCRIPT_OPCODE_SUBTRACT,
    SCRIPT_OPCODE_GREATER,
    SCRIPT_OPCODE_GREATER_EQUAL,
    SCRIPT_OPCODE_LESS,
    SCRIPT_OPCODE_LESS_EQUAL,
    SCRIPT_OPCODE_EQUAL,
    SCRIPT_OPCODE_NOT_EQUAL,
    SCRIPT_OPCODE_SET_VALUE,
    SCRIPT_OPCODE_COPY_STRUCT,
    SCRIPT_OPCODE_LOGICAL_AND,
    SCRIPT_OPCODE_LOGICAL_OR,
    SCRIPT_OPCODE_PRINTF,
    SCRIPT_OPCODE_YIELD,
    SCRIPT_OPCODE_CALL_STD,
    SCRIPT_OPCODE_COUNT,
} ScriptOpcodeId;

typedef union ScriptValue {
    s32 i;
    float f;
} ScriptValue;


typedef struct ScriptModule {
    u16 m_moduleId;
    u16 m_reserved;
    u16 m_functionCount;
    u16 m_relocationCount;
    u16 m_variableCount;
    u8 m_relocated;
    u32 m_relocationTableOffset;
    u32 m_variablesOffset;
    struct ScriptModule *m_pNextModule;
    u32 m_functionOffsets[1];
} ScriptModule;

typedef struct ScriptState {
    ScriptModule *m_pModule;
    ScriptStateCode m_state;
    u8 m_reserved;
    u16 m_taskSlot;
    u32 m_scriptId;
    void *m_pTaskContext;
    ScriptTaskCallback m_pCallback;
    u8 *m_pCode;
    ScriptValue *m_pVariables;
    s32 m_frameBase;
    u32 m_reservedFrame[2];
    s32 m_sp;
    u32 m_reservedTail[16];
    ScriptValue m_stack[SCRIPT_STACK_SIZE];
} ScriptState;

extern ScriptOpcode scriptOpcodeHandlers[SCRIPT_OPCODE_COUNT];

typedef s32 (*ScriptStdFunctionPtr)();

typedef struct ScriptStdFunction {
    ScriptStdFunctionPtr func;
    u8 argTypes[SCRIPT_STD_MAX_ARGS];
} ScriptStdFunction;

typedef struct ScriptStdLibrary {
    u16 m_taskCount;
    u16 m_memoryHandle;
    u16 m_nextTaskSlot;
    u16 m_reserved;
    ScriptModule *m_pModules;
    ScriptState *m_pStates;
    ScriptStdFunction *m_pFunctions;
} ScriptStdLibrary;

extern s32 scriptStdIntArgs[SCRIPT_STD_MAX_ARGS];
extern float scriptStdFloatArgs[SCRIPT_STD_MAX_ARGS];
extern char scriptOutputScratch[256];
extern char scriptFormatScratch[64];

extern ScriptStdLibrary *scriptStdLibrary;
extern ScriptStdFunctionPtr scriptCurrentStdFunction;
extern float *scriptFloatArguments;
extern s32 *scriptIntArguments;
extern s32 scriptInvokeStdFunction(void);

extern s32 scriptNop(ScriptState *self);
extern s32 scriptEnd(ScriptState *self);
extern s32 scriptPushMinusOne(ScriptState *self);
extern s32 scriptPop(ScriptState *self);
extern s32 scriptCall(ScriptState *self);
extern s32 scriptReturn(ScriptState *self);
extern s32 scriptJump(ScriptState *self);
extern s32 scriptJumpFalse(ScriptState *self);
extern s32 scriptPushWord(ScriptState *self);
extern s32 scriptBitwiseAnd(ScriptState *self);
extern s32 scriptBitwiseOr(ScriptState *self);
extern s32 scriptPushVar(ScriptState *self);
extern s32 scriptPushMultiVar(ScriptState *self);
extern s32 scriptLogicalNot(ScriptState *self);
extern s32 scriptNegate(ScriptState *self);
extern s32 scriptPushVar2(ScriptState *self);
extern s32 scriptMultiply(ScriptState *self);
extern s32 scriptDivide(ScriptState *self);
extern s32 scriptModulo(ScriptState *self);
extern s32 scriptAdd(ScriptState *self);
extern s32 scriptSubtract(ScriptState *self);
extern s32 scriptLess(ScriptState *self);
extern s32 scriptLessEqual(ScriptState *self);
extern s32 scriptGreater(ScriptState *self);
extern s32 scriptGreaterEqual(ScriptState *self);
extern s32 scriptEqual(ScriptState *self);
extern s32 scriptNotEqual(ScriptState *self);
extern s32 scriptSetValue(ScriptState *self);
extern s32 scriptCopyStruct(ScriptState *self);
extern s32 scriptLogicalAnd(ScriptState *self);
extern s32 scriptLogicalOr(ScriptState *self);
extern s32 scriptPrintOpcode(ScriptState *self);
extern s32 scriptYield(ScriptState *self);
extern s32 scriptCallStd(ScriptState *self);
extern s32 scriptRun(ScriptState *self);

#endif
