/* symbols just to satisfy the linker*/
/* dolsdk2001's dummy link-satisfier TU: one-line dummy bodies for the symbols
 * the SDK archives expect some other object to provide. The retail DOL never
 * links it, so every definition below is unused:
 *   43 of the 71 exports duplicate code the DOL already contains (memcpy,
 *      memset and __init_registers in .init, main at 0x800055E0, and the
 *      __div2i, __mod2i, __cvt_fp2unsigned, str and mem families, the printf
 *      family, DSPAddTask, the MetroTRK entries and the _savefpr/_restfpr
 *      helpers in the .text slice at 0x800A2B9C);
 *    8 are already provided outside any object's text: _stack_addr,
 *      _stack_end, __ArenaLo and __ArenaHi by build/GC6E01/ldscript.lcf, and
 *      _rom_copy_info, _bss_init_info, _ctors and _dtors by the DOL's own
 *      .init/.ctors/.dtors tables;
 *   20 have no DOL counterpart at all: DSPAssertTask, DSPCheckInit,
 *      DSPCheckMailToDSP, DSPGetDMAStatus, DSPHalt, DSPInit, DSPReset,
 *      DSPSendMailToDSP, __shl2i, __shr2i, __shr2u, _start, cosf, powf, sinf,
 *      tanf, rand, srand, tolower and vsprintf.
 * Linking the compiled object would therefore be a 51-way duplicate-definition
 * error and would insert 0x11C bytes of .text the DOL does not contain, so
 * config/GC6E01/libs.toml registers it with completed = false and it has no
 * split range in config/GC6E01/splits.txt.
 */
#define TEXT_STUB(name) void name(void) {}
#define DATA_STUB(name) int name;

// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_start)

// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__ArenaHi)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__ArenaLo)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__cvt_fp2unsigned)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__div2i)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__init_registers)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__mod2i)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__shl2i)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__shr2i)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(__shr2u)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_ctors)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_dtors)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_bss_init_info)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_18)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_19)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_20)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_22)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_23)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_24)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_25)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_26)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_27)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_28)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_29)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_restfpr_30)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_rom_copy_info)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_18)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_19)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_20)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_22)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_23)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_24)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_25)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_26)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_27)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_28)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_29)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_savefpr_30)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_stack_addr)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(_stack_end)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPAddTask)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPAssertTask)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPCheckInit)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPCheckMailToDSP)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPGetDMAStatus)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPHalt)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPInit)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPReset)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(DSPSendMailToDSP)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(EnableMetroTRKInterrupts)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(InitMetroTRK)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(cosf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(main)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(memcmp)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(memcpy)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(memmove)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(memset)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(powf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(rand)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(sinf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(sprintf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(srand)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(strchr)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(strcmp)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(strcpy)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(strlen)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(strncpy)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(tanf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(tolower)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(vprintf)
// UNUSED: never linked; the retail DOL defines this symbol elsewhere or not at all.
TEXT_STUB(vsprintf)
