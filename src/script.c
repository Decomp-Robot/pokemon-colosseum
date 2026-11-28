#include "pokemon/types.h"
#include "GSAPI/GSdebug.h"
#include "GSAPI/GSgfx/GSgfx.h"
#include "app/task/task.h"
#include "app/script/script.h"
#include "app/script/scriptMath.h"
#include <stdio.h>
#include <string.h>

extern s32 scriptDefaultOperand;

const char scriptStackOverflowMessage[] = "Stack overflow.\n";

static inline void ScriptPushInt(ScriptState *self, s32 value);

static inline ScriptValue ScriptPopValue(ScriptState *self)
{
	ScriptValue value;

	if (self->m_sp <= 0) {
		GSdebugPrintf("Stack underflow.\n");
		value.i = self->m_stack[0].i;
	} else {
		value.i = self->m_stack[--self->m_sp].i;
	}
	return value;
}

static inline ScriptValue ScriptResolveOperand(ScriptState *self, u16 desc)
{
	ScriptValue value;
	s32 index;
	s32 *ptr;

	value.i = scriptDefaultOperand;
	if (desc == 0) {
		GSdebugPrintf("変数の型が未定義です:[%02x]\n", desc);
		return value;
	}
	if (desc & SCRIPT_OPERAND_INDIRECT) {
		value = ScriptPopValue(self);
	} else {
		index = ScriptPopValue(self).i;
		if (desc & SCRIPT_OPERAND_VARIABLE) {
			if (desc & SCRIPT_OPERAND_LOCAL)
				value.i = (s32)&self->m_stack[self->m_frameBase + index];
			else
				value.i = (s32)&self->m_pVariables[index];
		} else {
			if (desc & SCRIPT_OPERAND_LOCAL)
				ptr = &self->m_stack[self->m_frameBase + index].i;
			else
				ptr = &self->m_pVariables[index].i;
			if (desc & SCRIPT_OPERAND_ADDRESS)
				value.i = (s32)ptr;
			else
				value.i = *ptr;
		}
	}
	return value;
}

static inline void ScriptEnterFrame(ScriptState *self)
{
	u16 argCount;

	self->m_pCode += 2;
	argCount = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	ScriptPushInt(self, self->m_frameBase);
	ScriptPushInt(self, argCount);
	self->m_frameBase = self->m_sp - (argCount + 2);
}

static inline ScriptValue *ScriptGetArgument(ScriptState *self, s32 index)
{
	return &self->m_stack[self->m_frameBase + index];
}

static inline void ScriptLeaveFrame(ScriptState *self)
{
	ScriptValue count;
	ScriptValue frameBase;
	s32 i;

	count = ScriptPopValue(self);
	frameBase = ScriptPopValue(self);
	self->m_frameBase = frameBase.i;
	for (i = 0; i < count.i; i++)
		ScriptPopValue(self);
}

static inline void ScriptPushOperand(ScriptState *self, ScriptValue value)
{
	if (self->m_sp > SCRIPT_STACK_SIZE) {
		GSdebugPrintf(scriptStackOverflowMessage);
		return;
	}
	self->m_stack[self->m_sp++] = value;
}

static inline void ScriptPushInt(ScriptState *self, s32 value)
{
	if (self->m_sp > SCRIPT_STACK_SIZE) {
		GSdebugPrintf(scriptStackOverflowMessage);
		return;
	}
	self->m_stack[self->m_sp++].i = value;
}

static inline u32 ScriptOperandType(u16 desc)
{
	return desc & SCRIPT_VALUE_MASK;
}

static inline void ScriptSetValue(ScriptState *self, u16 desc, ScriptValue value)
{
	if (desc == 0) {
		GSdebugPrintf("変数の型が未定義:[%02x]\n", desc);
	} else if (desc & SCRIPT_OPERAND_INDIRECT) {
		GSdebugPrintf("setValue 例外エラー：定数に数値のセットはできません\n");
	} else {
		s32 index = ScriptPopValue(self).i;

		if (desc & SCRIPT_OPERAND_LOCAL)
			self->m_stack[self->m_frameBase + index] = value;
		else
			self->m_pVariables[index] = value;
	}
}

static inline void ScriptDumpStack(ScriptState *self)
{
	s32 i;

	if (self->m_sp > 0) {
		for (i = self->m_sp - 1; i >= 0; i--) {
#ifdef DEBUG
			GSdebugPrintfNoFloat("stack[%d] = %d\n", i, self->m_stack[i].i);
#endif
		}
	}
}

s32 scriptCallStd(ScriptState *self)
{
	ScriptStdFunction *function;
	s32 result;
	s32 count;
	s32 i;

	result = 0;
	ScriptEnterFrame(self);

	function = &scriptStdLibrary->m_pFunctions[ScriptGetArgument(self, 0)->i];
	scriptCurrentStdFunction = function->func;
	if (scriptCurrentStdFunction != 0) {
		s32 floatCount = 0;
		s32 intCount = 0;

		memset(scriptStdIntArgs, 0, sizeof(scriptStdIntArgs));
		memset(scriptStdFloatArgs, 0, sizeof(scriptStdFloatArgs));
		count = self->m_sp - self->m_frameBase - 3;
		if (count > SCRIPT_STD_MAX_ARGS)
			count = SCRIPT_STD_MAX_ARGS;
		for (i = 0; i < count; i++) {
			if (function->argTypes[i] == SCRIPT_STD_ARG_END)
				break;
			if (function->argTypes[i] == SCRIPT_STD_ARG_FLOAT)
				scriptStdFloatArgs[floatCount++] = ScriptGetArgument(self, i + 1)->f;
			else
				scriptStdIntArgs[intCount++] = ScriptGetArgument(self, i + 1)->i;
		}
		scriptFloatArguments = scriptStdFloatArgs;
		scriptIntArguments = scriptStdIntArgs;
		result = scriptInvokeStdFunction();
	}
	self->m_stack[self->m_frameBase - 1].i = result;

	ScriptLeaveFrame(self);
	return 1;
}

s32 scriptYield(ScriptState *self)
{
	s32 frames;
	s32 elapsed;

	ScriptEnterFrame(self);
	frames = ScriptGetArgument(self, 0)->i;
	elapsed = 0;
	self->m_state = SCRIPT_STATE_YIELDING;
	while (elapsed < frames) {
		if (self->m_state == SCRIPT_STATE_TERMINATE_REQUEST)
			self->m_state = SCRIPT_STATE_TERMINATE;
		if (self->m_state == SCRIPT_STATE_TERMINATE) {
			ScriptLeaveFrame(self);
			return 0;
		}
		waitFrame();
		elapsed += GSgetElapsedFrames();
	}
	self->m_state = SCRIPT_STATE_RESUMED;
	ScriptLeaveFrame(self);
	return 1;
}

s32 scriptPrintOpcode(ScriptState *self)
{
	s32 length;
	s32 argIndex;
	char *format;
	char *out;
	ScriptValue arg;

	ScriptEnterFrame(self);
	argIndex = 1;
	out = scriptOutputScratch;
	arg = *ScriptGetArgument(self, 0);
	format = (char *)arg.i;
	while (*format) {
		if (*format == '%') {
			s32 specLength = 0;

			arg = *ScriptGetArgument(self, argIndex);
			argIndex++;
			while (1) {
				scriptFormatScratch[specLength++] = *format;
				if (*format == 'd' || *format == 'x' || *format == 'c') {
					scriptFormatScratch[specLength] = '\0';
					length = sprintf(out, scriptFormatScratch, arg.i);
					break;
				} else if (*format == 'f') {
					scriptFormatScratch[specLength] = '\0';
					length = sprintf(out, scriptFormatScratch, arg.f);
					break;
				} else if (*format == 's') {
					scriptFormatScratch[specLength] = '\0';
					length = sprintf(out, scriptFormatScratch, arg.i);
					break;
				} else if (*format == '\0') {
					break;
				}
				format++;
			}
			out += length;
		} else if (*format == '\\') {
			switch (format[1]) {
			case 'n':
				length = sprintf(out, "\n");
				format++;
				out += length;
				break;
			default:
				*out++ = *format;
				break;
			}
		} else {
			*out++ = *format;
		}
		format++;
		if (out >= scriptOutputScratch + 255)
			break;
	}
	*out = '\0';
	GSdebugPrintfNoFloat(scriptOutputScratch);
	ScriptLeaveFrame(self);
	return 1;
}

s32 scriptLogicalOr(ScriptState *self)
{
	u8 descA;
	u8 descB;
	ScriptValue valueA;
	ScriptValue valueB;
	ScriptValue result;

	descB = *self->m_pCode++;
	descA = *self->m_pCode++;
	valueB = ScriptResolveOperand(self, descB);
	valueA = ScriptResolveOperand(self, descA);
	if (ScriptOperandType(descA) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (valueA.i || valueB.i) ? 1 : 0;
		else
			result.i = (valueA.i || valueB.f) ? 1 : 0;
	} else {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (valueA.f || valueB.i) ? 1 : 0;
		else
			result.i = (valueA.f || valueB.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptLogicalAnd(ScriptState *self)
{
	u8 descA;
	u8 descB;
	ScriptValue valueA;
	ScriptValue valueB;
	ScriptValue result;

	descB = *self->m_pCode++;
	descA = *self->m_pCode++;
	valueB = ScriptResolveOperand(self, descB);
	valueA = ScriptResolveOperand(self, descA);
	if (ScriptOperandType(descA) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (valueA.i && valueB.i) ? 1 : 0;
		else
			result.i = (valueA.i && valueB.f) ? 1 : 0;
	} else {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (valueA.f && valueB.i) ? 1 : 0;
		else
			result.i = (valueA.f && valueB.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptCopyStruct(ScriptState *self)
{
	u8 srcDesc;
	u8 dstDesc;
	u16 count;
	ScriptValue *src;
	ScriptValue *dst;
	s32 index;
	s32 i;

	srcDesc = *self->m_pCode++;
	dstDesc = *self->m_pCode++;
	count = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	if (dstDesc & SCRIPT_OPERAND_INDIRECT) {
		GSdebugPrintf("_codeStructAssign:  エラー：定数に数値のセットはできません\n");
		return 1;
	}
	if (srcDesc & SCRIPT_OPERAND_INDIRECT) {
		self->m_sp -= count;
		src = &self->m_stack[self->m_sp];
	} else {
		index = ScriptPopValue(self).i;
		if (srcDesc & SCRIPT_OPERAND_LOCAL)
			src = &self->m_stack[self->m_frameBase + index];
		else
			src = &self->m_pVariables[index];
	}
	index = ScriptPopValue(self).i;
	if (dstDesc & SCRIPT_OPERAND_LOCAL)
		dst = &self->m_stack[self->m_frameBase + index];
	else
		dst = &self->m_pVariables[index];
	for (i = 0; i < count; i++)
		dst[i] = src[i];
	for (i = 0; i < count; i++)
		ScriptPushOperand(self, dst[i]);
	return 1;
}

s32 scriptSetValue(ScriptState *self)
{
	u8 dstDesc;
	u8 srcDesc;
	ScriptValue value;

	srcDesc = *self->m_pCode++;
	dstDesc = *self->m_pCode++;
	value = ScriptResolveOperand(self, srcDesc);
	if ((dstDesc & SCRIPT_VALUE_MASK) != (srcDesc & SCRIPT_VALUE_MASK)) {
		if ((srcDesc & SCRIPT_VALUE_MASK) == SCRIPT_VALUE_INT)
			value.f = (float)value.i;
		else
			value.i = (s32)value.f;
	}
	ScriptSetValue(self, dstDesc, value);
	ScriptPushOperand(self, value);
	return 1;
}

s32 scriptNotEqual(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i != right.i;
		else
			result.i = ((float)left.i != right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f != (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f != right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptEqual(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i == right.i;
		else
			result.i = ((float)left.i == right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f == (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f == right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptLessEqual(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i >= right.i;
		else
			result.i = ((float)left.i >= right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f >= (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f >= right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptLess(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i > right.i;
		else
			result.i = ((float)left.i > right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f > (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f > right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptGreaterEqual(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i <= right.i;
		else
			result.i = ((float)left.i <= right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f <= (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f <= right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptGreater(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i < right.i;
		else
			result.i = ((float)left.i < right.f) ? 1 : 0;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.i = (left.f < (float)right.i) ? 1 : 0;
	} else {
		result.i = (left.f < right.f) ? 1 : 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptSubtract(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i - right.i;
		else
			result.f = (float)left.i - right.f;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.f = left.f - (float)right.i;
	} else {
		result.f = left.f - right.f;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptAdd(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i + right.i;
		else
			result.f = (float)left.i + right.f;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.f = left.f + (float)right.i;
	} else {
		result.f = left.f + right.f;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptModulo(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (right.i != 0) {
		if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
			if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
				result.i = left.i % right.i;
			else
				result.f = fmod(left.i, right.f);
		} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
			result.f = fmod(left.i, right.f);
		} else {
			result.f = fmod(left.i, right.f);
		}
	} else {
		GSdebugPrintf("０で除算しようとしました\n");
		result.i = 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptDivide(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (right.i != 0) {
		if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
			if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
				result.i = left.i / right.i;
			else
				result.f = (float)left.i / right.f;
		} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
			result.f = left.f / (float)right.i;
		} else {
			result.f = left.f / right.f;
		}
	} else {
		GSdebugPrintf("０で除算しようとしました\n");
		result.i = 0;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptMultiply(ScriptState *self)
{
	u8 descLeft;
	u8 descRight;
	ScriptValue left;
	ScriptValue right;
	ScriptValue result;

	descRight = *self->m_pCode++;
	descLeft = *self->m_pCode++;
	right = ScriptResolveOperand(self, descRight);
	left = ScriptResolveOperand(self, descLeft);
	if (ScriptOperandType(descLeft) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT)
			result.i = left.i * right.i;
		else
			result.f = (float)left.i * right.f;
	} else if (ScriptOperandType(descRight) == SCRIPT_VALUE_INT) {
		result.f = left.f * (float)right.i;
	} else {
		result.f = left.f * right.f;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptPushVar2(ScriptState *self)
{
	u8 desc = *self->m_pCode++;

	ScriptPushOperand(self, ScriptResolveOperand(self, desc));
	return 1;
}

s32 scriptNegate(ScriptState *self)
{
	u8 desc;
	ScriptValue value;
	ScriptValue result;

	desc = *self->m_pCode++;
	value = ScriptResolveOperand(self, desc);
	if ((desc & SCRIPT_VALUE_MASK) == SCRIPT_VALUE_INT)
		result.i = -value.i;
	else
		result.f = -value.f;
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptLogicalNot(ScriptState *self)
{
	u8 desc;
	ScriptValue value;
	ScriptValue result;

	desc = *self->m_pCode++;
	value = ScriptResolveOperand(self, desc);
	if ((desc & SCRIPT_VALUE_MASK) == SCRIPT_VALUE_INT)
		result.i = !value.i;
	else
		result.f = !value.f;
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptPushMultiVar(ScriptState *self)
{
	s32 i;
	s32 count;
	u8 desc;
	s32 base;
	s32 index;
	u16 type;

	desc = *self->m_pCode++;
	count = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	base = ScriptPopValue(self).i;
	for (i = 0; i < count; i++) {
		index = base + i;
		type = desc;
		ScriptPushInt(self, index);
		ScriptPushOperand(self, ScriptResolveOperand(self, type));
	}
	return 1;
}

s32 scriptPushVar(ScriptState *self)
{
	u8 desc;
	u16 addressDesc;
	s32 index;

	desc = *self->m_pCode++;
	index = ScriptPopValue(self).i;
	ScriptPushInt(self, index);
	addressDesc = desc | SCRIPT_OPERAND_ADDRESS;
	ScriptPushOperand(self, ScriptResolveOperand(self, addressDesc));
	return 1;
}

s32 scriptBitwiseOr(ScriptState *self)
{
	u8 descA;
	u8 descB;
	ScriptValue valueA;
	ScriptValue valueB;
	ScriptValue result;

	descB = *self->m_pCode++;
	descA = *self->m_pCode++;
	valueB = ScriptResolveOperand(self, descB);
	valueA = ScriptResolveOperand(self, descA);
	if (ScriptOperandType(descA) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = valueA.i | valueB.i;
		else
			result.i = valueA.i | (s32)valueB.f;
	} else {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (s32)valueA.f | valueB.i;
		else
			result.i = (s32)valueA.f | (s32)valueB.f;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptBitwiseAnd(ScriptState *self)
{
	u8 descA;
	u8 descB;
	ScriptValue valueA;
	ScriptValue valueB;
	ScriptValue result;

	descB = *self->m_pCode++;
	descA = *self->m_pCode++;
	valueB = ScriptResolveOperand(self, descB);
	valueA = ScriptResolveOperand(self, descA);
	if (ScriptOperandType(descA) == SCRIPT_VALUE_INT) {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = valueA.i & valueB.i;
		else
			result.i = valueA.i & (s32)valueB.f;
	} else {
		if (ScriptOperandType(descB) == SCRIPT_VALUE_INT)
			result.i = (s32)valueA.f & valueB.i;
		else
			result.i = (s32)valueA.f & (s32)valueB.f;
	}
	ScriptPushOperand(self, result);
	return 1;
}

s32 scriptPushWord(ScriptState *self)
{
	u32 word;

	self->m_pCode++;
	word = *(u32 *)self->m_pCode;
	ScriptPushInt(self, word);
	self->m_pCode += 4;
	return 1;
}

s32 scriptJumpFalse(ScriptState *self)
{
	s32 value = ScriptPopValue(self).i;

	if (value != 0)
		self->m_pCode += 4;
	else
		self->m_pCode = (u8 *)self->m_pModule + *(u32 *)self->m_pCode;
	return 1;
}

s32 scriptJump(ScriptState *self)
{
	self->m_pCode = (u8 *)self->m_pModule + *(u32 *)self->m_pCode;
	return 1;
}

s32 scriptReturn(ScriptState *self)
{
	ScriptValue argCount = ScriptPopValue(self);
	ScriptValue frameBase = ScriptPopValue(self);
	ScriptValue returnAddress;
	s32 i;

	self->m_frameBase = frameBase.i;
	returnAddress = ScriptPopValue(self);
	self->m_pCode = (u8 *)returnAddress.i;
	for (i = 0; i < argCount.i; i++)
		ScriptPopValue(self);
	if (self->m_pCode == 0) {
		self->m_state = SCRIPT_STATE_TERMINATE;
		return 0;
	}
	return 1;
}

s32 scriptCall(ScriptState *self)
{
	u32 function = *(u32 *)self->m_pCode;
	u16 argCount;
	u16 localCount;

	self->m_pCode += 4;
	argCount = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	localCount = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	ScriptPushInt(self, (s32)self->m_pCode);
	ScriptPushInt(self, self->m_frameBase);
	ScriptPushInt(self, localCount);
	self->m_frameBase = self->m_sp - (argCount + localCount + 3);
	if (function >= self->m_pModule->m_functionCount)
		GSdebugPrintf("設定する関数のインデックス[%d]が不正です\n", function);
	else
		self->m_pCode = (u8 *)self->m_pModule + self->m_pModule->m_functionOffsets[function];
	return 1;
}

s32 scriptPop(ScriptState *self)
{
	s32 count;
	s32 i;

	self->m_pCode++;
	count = *(u16 *)self->m_pCode;
	self->m_pCode += 2;
	for (i = 0; i < count; i++)
		ScriptPopValue(self);
	return 1;
}

s32 scriptPushMinusOne(ScriptState *self)
{
	ScriptPushInt(self, -1);
	return 1;
}

s32 scriptEnd(ScriptState *self)
{
	self->m_state = SCRIPT_STATE_TERMINATE;
	return 0;
}

s32 scriptNop(ScriptState *self)
{
	return 1;
}

s32 scriptRun(ScriptState *self)
{
	u8 opcode;
	s32 result;

	while (1) {
		if (self->m_state == SCRIPT_STATE_STANDBY) {
			GSdebugPrintfNoFloat("script %08X terminated by status == STANDBY\n", self->m_scriptId);
			break;
		}
		if (self->m_state == SCRIPT_STATE_TERMINATE) {
			self->m_state = SCRIPT_STATE_STANDBY;
			break;
		}
		opcode = *self->m_pCode++;
		if ((s32)opcode >= SCRIPT_OPCODE_COUNT) {
			GSdebugPrintf("サポートされていないコードです:%02x\n", opcode);
		} else if (scriptOpcodeHandlers[opcode] != 0) {
			scriptOpcodeHandlers[opcode](self);
		}
		ScriptDumpStack(self);
	}
	if (self->m_state == SCRIPT_STATE_TERMINATE_REQUEST)
		self->m_state = SCRIPT_STATE_STANDBY;
	result = ScriptPopValue(self).i;
	if (self->m_pCallback != 0)
		self->m_pCallback(self, result);
	return result;
}

s32 scriptStdIntArgs[SCRIPT_STD_MAX_ARGS];
float scriptStdFloatArgs[SCRIPT_STD_MAX_ARGS];
char scriptOutputScratch[256];
char scriptFormatScratch[64];
