
#include "musyx/assert.h"
#include "musyx/hardware.h"
#include "musyx/s3d.h"
#include "musyx/seq.h"
#include "musyx/stream.h"
#include "musyx/synth.h"
#include "musyx/synthdata.h"
#include "musyx/vsamples.h"

#if MUSY_VERSION < MUSY_VERSION_CHECK(1, 5, 2)
static inline s32 DoInit(u32 mixFrq, u32 aramSize, u32 numVoices)
#else
static inline s32 DoInit(u32 mixFrq, u32 aramSize, u32 numVoices, u32 flags)
#endif
{
  MUSY_DEBUG("\nMusyX software initialization...\nBuild Date: %s %s\n\n", __DATE__, __TIME__);

  dataInitStack();
  dataInit(0, aramSize);

  seqInit();

  synthIdleWaitActive = FALSE;

  synthInit(mixFrq, numVoices);

  streamInit();

#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 3)
  vsInit();
#endif
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 2)
  s3dInit(flags);
#else
  s3dInit();
#endif

  sndActive = TRUE;

  MUSY_DEBUG("MusyX logic is initialized.\n\n");

  return 0;
}

#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
s32 sndInit(u8 voices, u8 music, u8 sfx, u8 studios, u32 flags, u32 aramSize) {
#else
s32 sndInit(u8 voices, u8 music, u8 sfx, u32 flags, u32 aramSize) {
#endif
  s32 ret;
  u32 frq;

  MUSY_DEBUG("Entering sndInit()\n\n");
  sndActive = FALSE;
  if (voices <= SYNTH_MAX_VOICES) {
    synthInfo.voiceNum = voices;
  } else {
    synthInfo.voiceNum = SYNTH_MAX_VOICES;
  }
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if (studios <= SND_STUDIO_MAXNUM) {
    synthInfo.studioNum = studios;
  } else {
    synthInfo.studioNum = SND_STUDIO_MAXNUM;
  }
#endif

  synthInfo.maxMusic = music;
  synthInfo.maxSFX = sfx;
  frq = 32000;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if ((ret = hwInit(&frq, synthInfo.voiceNum, synthInfo.studioNum, flags)) == 0) {
#else
  if ((ret = hwInit(&frq, synthInfo.voiceNum, flags)) == 0) {
#endif
#if MUSY_VERSION < MUSY_VERSION_CHECK(1, 5, 2)
    ret = DoInit(32000, aramSize, synthInfo.voiceNum);
#else
    ret = DoInit(32000, aramSize, synthInfo.voiceNum, flags);
#endif
  }

  MUSY_DEBUG("Leaving sndInit().\n\n");
  return ret;
}

void sndQuit(void) {
  MUSY_ASSERT_MSG(sndActive, "Sound system is not initialized.");

  hwExit();
  dataExit();
  s3dExit();
  synthExit();
  sndActive = FALSE;
}

bool sndIsInstalled(void) { return sndActive; }
