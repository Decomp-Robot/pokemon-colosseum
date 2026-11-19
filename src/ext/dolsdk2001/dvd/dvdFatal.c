#include <dolphin.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>
#include <dolphin/vi.h>

#include "__dvd.h"

static const char* JapanMessage =
    "\n\n\nエラーが発生しました。\n\n"
    "本体のパワーボタンを押して電源をOFFにし、\n"
    "本体の取扱説明書の指示に従ってください。";
static const char* UsaMessage = "\n\n\nAn error has occurred.\nTurn the power off and refer to the\nNintendo GameCube Instruction Booklet\nfor further instructions.";

const char* const Europe[] = {
    /* English (OSGetLanguage() == 0) */
    "\n\n\nAn error has occurred.\nTurn the power off and refer to the\nNintendo GameCube\231 Instruction Booklet\nfor further instructions.",
    /* German (OSGetLanguage() == 1) */
    "\n\n\nEin Fehler ist aufgetreten.\nBitte schalten Sie den NINTENDO GAMECUBE\naus und lesen Sie die Bedienungsanleitung,\num weitere Informationen zu erhalten.",
    /* French (OSGetLanguage() == 2) */
    "\n\n\nUne erreur est survenue.\nEteignez la console et r\351f\351rez-vous au\nmanuel d'instructions NINTENDO GAMECUBE\npour de plus amples informations.",
    /* Spanish (OSGetLanguage() == 3) */
    "\n\n\nSe ha producido un error.\nApaga la consola y consulta el manual\nde instrucciones de NINTENDO GAMECUBE\npara obtener m\341s informaci\363n.",
    /* Italian (OSGetLanguage() == 4) */
    "\n\n\nSi \350 verificato un errore.\nSpegni (OFF) e controlla il manuale\nd'istruzioni del NINTENDO GAMECUBE\nper ulteriori indicazioni.",
    /* Dutch (OSGetLanguage() == 5) */
    "\n\n\nEr is een fout opgetreden.\nZet de NINTENDO GAMECUBE uit en\nraadpleeg de handleiding van de\nNintendo GameCube voor nadere\ninstructies.",
};

static void (*AutoFatalMessaging)(void);

static void ShowMessage(void) {
    GXColor bg = {0, 0, 0, 0};
    GXColor fg = {255, 255, 255, 0};
    const char* message;

    if (VIGetTvFormat() == VI_NTSC) {
        if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
            message = JapanMessage;
        } else {
            message = UsaMessage;
        }
    } else {
        message = Europe[OSGetLanguage()];
    }

    OSFatal(fg, bg, message);
}

BOOL DVDSetAutoFatalMessaging(BOOL enable) {
    BOOL enabled;
    BOOL installed;

    enabled = OSDisableInterrupts();
    installed = AutoFatalMessaging ? TRUE : FALSE;
    AutoFatalMessaging = enable ? ShowMessage : NULL;
    OSRestoreInterrupts(enabled);
    return installed;
}

void __DVDPrintFatalMessage(void) {
    if (AutoFatalMessaging != NULL) {
        AutoFatalMessaging();
    }
}
