#include "musyx/musyx.h"
#include "musyx/assert.h"
#include "musyx/hardware.h"
#include "musyx/aramdma.h"
#include "musyx/s3d.h"
#include "musyx/seq.h"
#include "musyx/voice.h"
#include "musyx/synth.h"
#include "musyx/synthdata.h"

enum DataType {
  DATA_TYPE_MACRO,
  DATA_TYPE_SAMPLE,
  DATA_TYPE_KEYMAP,
  DATA_TYPE_LAYER,
  DATA_TYPE_CURVE,
};

#define POOL_LIST_END 0xFFFFFFFF
#define GROUP_LIST_END 0xFFFFFFFF
#define MIDI_SETUP_LIST_END 0xFFFF

#define GROUP_STACK_MAX 128

enum GroupType {
  GROUP_TYPE_SONG,
  GROUP_TYPE_FX,
};

static GSTACK gs[GROUP_STACK_MAX];
static s16 sp;

void dataInitStack() { sp = 0; }

static inline MEM_DATA* FindPoolEntry(u16 id, MEM_DATA* m) {
  while (m->nextOff != POOL_LIST_END) {
    if (m->id == id) {
      return m;
    }

    m = (MEM_DATA*)((u8*)m + m->nextOff);
  }
  return NULL;
}

static inline MEM_DATA* FindMacroEntry(u16 id, POOL_DATA* pool) {
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if (pool == NULL) {
    return NULL;
  }
#endif
  return FindPoolEntry(id, (MEM_DATA*)((u8*)pool + pool->macroOff));
}

static inline MEM_DATA* FindCurveEntry(u16 id, POOL_DATA* pool) {
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if (pool == NULL) {
    return NULL;
  }
#endif
  return FindPoolEntry(id, (MEM_DATA*)((u8*)pool + pool->curveOff));
}

static inline MEM_DATA* FindKeymapEntry(u16 id, POOL_DATA* pool) {
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if (pool == NULL) {
    return NULL;
  }
#endif
  return FindPoolEntry(id, (MEM_DATA*)((u8*)pool + pool->keymapOff));
}

static inline MEM_DATA* FindLayerEntry(u16 id, POOL_DATA* pool) {
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
  if (pool == NULL) {
    return NULL;
  }
#endif
  return FindPoolEntry(id, (MEM_DATA*)((u8*)pool + pool->layerOff));
}

static void InsertData(int id, void* data, u8 dataType, bool remove) {
  POOL_DATA* pool = data;

  switch (dataType) {
  case DATA_TYPE_MACRO:
    if (!remove) {
      MEM_DATA* m = FindMacroEntry(id, pool);
      if (m != NULL) {
        dataInsertMacro(id, m->data.cmd[0]);
      }
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
      else {
        dataInsertMacro(id, NULL);
      }
#endif
    } else {
      dataRemoveMacro(id);
    }
    break;
  case DATA_TYPE_KEYMAP:
    id |= DATA_ID_TAG_KEYMAP;
    if (!remove) {
      MEM_DATA* m = FindKeymapEntry(id, pool);
      if (m != NULL) {
        dataInsertKeymap(id, m->data.map);
      }
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
      else {
        dataInsertKeymap(id, NULL);
      }
#endif
    } else {
      dataRemoveKeymap(id);
    }
    break;
  case DATA_TYPE_LAYER:
    id |= DATA_ID_TAG_LAYER;
    if (!remove) {
      MEM_DATA* m = FindLayerEntry(id, pool);
      if (m != NULL) {
        dataInsertLayer(id, m->data.layer.entry, (u16)m->data.layer.num);
      }
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
      else {
        dataInsertLayer(id, NULL, 0);
      }
#endif
    } else {
      dataRemoveLayer(id);
    }
    break;
  case DATA_TYPE_CURVE:
    if (!remove) {
      MEM_DATA* m = FindCurveEntry(id, pool);
      if (m != NULL) {
        dataInsertCurve(id, m->data.tab
#if MUSY_TARGET == MUSY_TARGET_PC
                        ,
                        0
#endif
        );
      }
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
      else {
        dataInsertCurve(id, NULL
#if MUSY_TARGET == MUSY_TARGET_PC
                        ,
                        0
#endif
        );
      }
#endif
    } else {
      dataRemoveCurve(id);
    }
    break;
  case DATA_TYPE_SAMPLE:
    if (!remove) {
      dataAddSampleReference(id);
    } else {
      dataRemoveSampleReference(id);
    }
    break;
  }
}

static inline void ScanIDList(u16* ref, void* data, u8 dataType, bool remove) {
  while (*ref != DATA_REF_LIST_END) {
    if ((*ref & DATA_REF_RANGE)) {
      u32 rangeId = *ref & DATA_REF_RANGE_MASK;
      while ((u16)rangeId <= ref[1]) {
        InsertData(rangeId, data, dataType, remove);
        ++rangeId;
      }
      ref += 2;

    } else {
      InsertData(*ref++, data, dataType, remove);
    }
  }
}

static inline void InsertMacros(u16* ref, POOL_DATA* pool) {
  ScanIDList(ref, pool, DATA_TYPE_MACRO, FALSE);
}

static inline void InsertCurves(u16* ref, POOL_DATA* pool) {
  ScanIDList(ref, pool, DATA_TYPE_CURVE, FALSE);
}

static inline void InsertKeymaps(u16* ref, POOL_DATA* pool) {
  ScanIDList(ref, pool, DATA_TYPE_KEYMAP, FALSE);
}

static inline void InsertLayers(u16* ref, POOL_DATA* pool) {
  ScanIDList(ref, pool, DATA_TYPE_LAYER, FALSE);
}

static inline void InsertSamples(u16* ref, void* samples, SDIR_DATA* sdir) {
  samples = hwTransAddr(samples);
  if (dataInsertSDir(sdir, samples)) {
    ScanIDList(ref, sdir, DATA_TYPE_SAMPLE, FALSE);
  }
}

static inline void InsertFXTab(u16 gid, FX_DATA* fd) { dataInsertFX(gid, fd->fx, fd->num); }

#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 2)
void sndSetSampleDataUploadCallback(void* (*callback)(u32 offset, u32 bytes), u32 chunkSize) {
  hwSetSaveSampleCallback(callback, chunkSize);
}
#endif

bool sndPushGroup(void* prj_data, SND_GROUPID gid, void* samples, void* sampdir, void* pool) {
  MUSY_ASSERT_MSG(prj_data != NULL, "Project data pointer is NULL");
  MUSY_ASSERT_MSG(sampdir != NULL, "Sample directory pointer is NULL");

  if (sndActive && sp < GROUP_STACK_MAX) {
    u8* prj = prj_data;

    GROUP_DATA* g = prj_data;

    while (g->nextOff != GROUP_LIST_END) {
      if (g->id == gid) {
        gs[sp].gAddr = g;
        gs[sp].prjAddr = prj_data;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 1)
        gs[sp].sdirAddr = sampdir;
#endif
        InsertSamples((u16*)(prj + g->sampleOff), samples, sampdir);
        InsertMacros((u16*)(prj + g->macroOff), pool);
        InsertCurves((u16*)(prj + g->curveOff), pool);
        InsertKeymaps((u16*)(prj + g->keymapOff), pool);
        InsertLayers((u16*)(prj + g->layerOff), pool);
        if (g->type == GROUP_TYPE_FX) {
          InsertFXTab(gid, (FX_DATA*)(prj + g->data.fx.tableOff));
        }
        hwSyncSampleMem();
        ++sp;
        return TRUE;
      }

      g = (GROUP_DATA*)(prj + g->nextOff);
    }
  }

  MUSY_DEBUG("Group ID=%d could not be pushed.\n", gid);
  return FALSE;
}

u32 seqPlaySong(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para, u8 irq_call, u8 studio) {
  int i;

  MUSY_ASSERT_MSG(sndActive != FALSE, "Sound system is not initialized.");

  for (i = 0; i < sp; ++i) {
    if (gs[i].gAddr->id != sgid) {
      continue;
    }

    if (gs[i].gAddr->type == GROUP_TYPE_SONG) {
      GROUP_DATA* g = gs[i].gAddr;
      void* prj = gs[i].prjAddr;
      PAGE* norm = (PAGE*)((u32)prj + g->data.song.normpageOff);
      PAGE* drum = (PAGE*)((u32)prj + g->data.song.drumpageOff);
      MIDISETUP* midiSetup = (MIDISETUP*)((u32)prj + g->data.song.midiSetupOff);

      while (midiSetup->songId != MIDI_SETUP_LIST_END) {
        if (midiSetup->songId == sid) {
          u32 seqId;

          if (irq_call) {
            seqId = seqStartPlay(norm, drum, midiSetup, arrfile, para, studio, sgid);
          } else {
            hwDisableIrq();
            seqId = seqStartPlay(norm, drum, midiSetup, arrfile, para, studio, sgid);
            hwEnableIrq();
          }
          return seqId;
        }

        ++midiSetup;
      }

      MUSY_DEBUG("Song ID=%d is not in group ID=%d.\n", sid, sgid);
      return SND_ID_ERROR;
    } else {
      MUSY_DEBUG("Group ID=%d is no songgroup.\n", sgid);
      return SND_ID_ERROR;
    }
  }

  MUSY_DEBUG("Group ID=%d is not on any soundstack.\n", sgid);
  return SND_ID_ERROR;
}

SND_SEQID sndSeqPlayEx(SND_GROUPID sgid, SND_SONGID sid, void* arrfile, SND_PLAYPARA* para, u8 studio) {
  return seqPlaySong(sgid, sid, arrfile, para, FALSE, studio);
}
