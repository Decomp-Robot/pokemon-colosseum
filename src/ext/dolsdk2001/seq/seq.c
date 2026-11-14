#include <dolphin.h>
#include <dolphin/seq.h>

static u8 __SEQMidiEventLength[128] = {
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02,
    0x00, 0x00, 0x02, 0x01,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static SEQSEQUENCE * __SEQSequenceList;

// functions
static void __SEQPushSequenceList(SEQSEQUENCE * sequence);
static void __SEQRemoveSequenceFromList(SEQSEQUENCE * sequence);
static u32 __SEQGetIntTrack(SEQTRACK * track);
static void __SEQHandleSysExEvent(SEQTRACK * track);
static void __SEQSetTicksPerFrame(SEQTRACK * track, float bps);
static void __SEQTempoMetaEvent(SEQTRACK * track);
static void __SEQTrackEnd(SEQTRACK * track);
static void __SEQHandleMetaEvent(SEQTRACK * track);
static void __SEQHandleSynthEvent(struct SYNSYNTH * synth, SEQTRACK * track);
static void __SEQRunEvent(struct SYNSYNTH * synth, SEQTRACK * track);
static void __SEQInitTracks(SEQSEQUENCE * sequence, u8 * read, int tracks);
static void __SEQReadHeader(SEQSEQUENCE * sequence, u8 * midiStream);

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQPushSequenceList(SEQSEQUENCE * sequence) {
    int old;

    old = OSDisableInterrupts();
    if (__SEQSequenceList) {
        sequence->next = __SEQSequenceList;
    } else {
        sequence->next = NULL;
    }
    __SEQSequenceList = sequence;
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQRemoveSequenceFromList(SEQSEQUENCE * sequence) {
    int old;
    SEQSEQUENCE * thisSequence;
    SEQSEQUENCE * next;

    old = OSDisableInterrupts();
    thisSequence = __SEQSequenceList;
    __SEQSequenceList = NULL;

    while(thisSequence) {
        next = thisSequence->next;
        if (thisSequence != sequence) {
            __SEQPushSequenceList(thisSequence);
        }
        thisSequence = next;
    }
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
static u32 __SEQGetIntTrack(SEQTRACK * track) {
    u32 value;

    ASSERTLINE(0x6F, track);
    for(value = *track->current & 0x7F; *track->current & 0x80; value = (value << 7) + (*track->current & 0x7F)) {
        track->current+=1;
    }
    track->current+=1;
    return value;
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQHandleSysExEvent(SEQTRACK * track) {
    u32 length;

    ASSERTLINE(0x86, track);
    length = __SEQGetIntTrack(track);
    track->current += length;
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQSetTicksPerFrame(SEQTRACK * track, float bps) {
    SEQSEQUENCE * sequence;

    ASSERTLINE(0x94, track);
    sequence = track->sequence;
    track->beatsPerSec = bps;
    track->ticksPerFrame = (65536.0f * (160.0f / ((32000.0f / bps) / sequence->timeFormat)));
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQTempoMetaEvent(SEQTRACK * track) {
    u32 data;
    float beatsPerSec;

    data = *track->current; track->current+=1;
    data = (*track->current + (data * 0xFF)); track->current+=1;
    data = (*track->current + (data * 0xFF)); track->current+=1;
    beatsPerSec = 1000000 / (f32)data;
    __SEQSetTicksPerFrame(track, beatsPerSec);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQTrackEnd(SEQTRACK * track) {
    SEQSEQUENCE * sequence;

    ASSERTLINE(0xBE, track);
    sequence = track->sequence;
    sequence->tracksRunning--;
    track->state = 0;
    if (sequence->tracksRunning == 0) {
        sequence->end = 1;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQHandleMetaEvent(SEQTRACK * track) {
    u8 type;
    u32 length;
    int unused;

    length; // dumb fix
    ASSERTLINE(0xD1, track);
    type = *track->current; track->current+=1;
    switch(type) {
        case 0x2F:
            __SEQTrackEnd(track);
            return;
        case 0x51:
            __SEQGetIntTrack(track);
            __SEQTempoMetaEvent(track);
            return;
        default:
            length = __SEQGetIntTrack(track);
            track->current += length;
            return;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQHandleSynthEvent(struct SYNSYNTH * synth, SEQTRACK * track) {
    u8 ch[3];
    u32 bytes;
    void (* callback)(void *, u8);

    bytes = __SEQMidiEventLength[track->status - 0x80];
    ch[0] = track->status;
    switch(bytes) {
        case 0:
            break;
        case 1:
            ch[1] = *track->current; track->current+=1;
            break;
        case 2:
            ch[1] = *track->current; track->current+=1;
            ch[2] = *track->current; track->current+=1;
            break;
    }
    if (ch[0] & 0xB0) {
        callback = ((SEQSEQUENCE*)track->sequence)->callback[ch[1]];
        if(callback) {
           callback(track, ch[1]);
        }
    }
    SYNMidiInput(synth, ch);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQRunEvent(struct SYNSYNTH * synth, SEQTRACK * track) {
    u8 event;

    ASSERTLINE(0x126, synth);
    ASSERTLINE(0x127, track);
    event = *track->current;
    if (event >= 0x80) {
        track->status = event; track->current+=1;
    }
    switch(track->status) {
        case 0xF7:
        case 0xF0:
            __SEQHandleSysExEvent(track);
            break;
        case 0xFF:
            __SEQHandleMetaEvent(track);
            break;
        default:
            __SEQHandleSynthEvent(synth, track);
            break;
    }
    if (track->current >= track->end) {
        __SEQTrackEnd(track);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQInitTracks(SEQSEQUENCE* sequence, u8* read, int tracks) {
    int i;
    u8* p;
    u32 chunk;
    u32 bytes;
    SEQTRACK* track;

    i = 0;
    p = read;
    while (tracks) {
        while (1) {
            chunk = *(u32*)p;
            p += 4;
            bytes = *(u32*)p;
            p += 4;
            if (chunk == 'MTrk') {
                track = &sequence->track[i];
                track->sequence = sequence;
                track->start = p;
                track->end = &p[bytes];
                track->current = p;
                track->defaultTicksPerFrame = (u32)(65536.0f * (160.0f / (16000.0f / (f32)sequence->timeFormat)));
                track->state = 0;
                p += bytes;
                break;
            }
            p += bytes;
        }
        tracks--;
        i++;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SEQReadHeader(SEQSEQUENCE * sequence, u8 * midiStream) {
    u8 * read;
    u32 bytes;
    u32 fileType;

    read = midiStream;
    ASSERTMSGLINE(0x188, *(u32*)read == 'MThd', "!!!midiStream is not a valid MIDI file\n!!!");
    read+=4;
    bytes = *(u32*)read; read+=4;
    fileType = *(u16*)read; read+=2;
    sequence->nTracks = *(u16*)read; read+=2;
    sequence->timeFormat = *(s16*)read; read+=2;
    ASSERTMSGLINE(0x197, sequence->timeFormat >= 0, "!!!SEQ does not support SMPTE time!!!\n");
    bytes -= 6;
    read += bytes;
    switch(fileType) {
        case 0:
            sequence->nTracks = 1;
            __SEQInitTracks(sequence, read, 1);
            break;
        case 1:
            ASSERTMSGLINE(0x1AD, sequence->nTracks < 0x40, "exceeded SEQ_MAX_TRACKS, please increase SEQ_MAX_TRACKS\n");
            __SEQInitTracks(sequence, read, sequence->nTracks);
            break;
        default:
            ASSERTMSGLINE(0x1B5, 0, "!!!Invalid MIDI file type\n!!!");
            break;
    }
    sequence->tracksRunning = sequence->nTracks;
}

// UNUSED: the game never calls this; dead-stripped at link.
void SEQInit(void) {
    __SEQSequenceList = NULL;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void SEQRemoveSequence(SEQSEQUENCE * sequence) {
    ASSERTLINE(0x24B, sequence);
    __SEQRemoveSequenceFromList(sequence);
    SYNQuitSynth(&sequence->synth);
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void SEQSetState(SEQSEQUENCE * sequence, u32 state) {
    int i;

    ASSERTLINE(0x26D, sequence);

    switch(state) {
        case 1:
        case 2:
            if (sequence->state == 0) {
                int old;

                old = OSDisableInterrupts();
                for(i = 0; i < sequence->nTracks; i++) {
                    SEQTRACK * track = &sequence->track[i];
                    track->current = track->start;
                    track->ticksPerFrame = track->defaultTicksPerFrame;
                    track->delay = __SEQGetIntTrack(track) << 0x10;
                    track->state = 1;                    
                }
                sequence->tracksRunning = sequence->nTracks;
                OSRestoreInterrupts(old);
            }
            sequence->end = 0;
            break;
        case 0:
        case 3: {
            int old;
            u8 ch[3];

            for(i = 0; i < 16; i++) {
                old = OSDisableInterrupts();
                ch[0] = (i | 0xB0);
                ch[1] = 0x7B;
                ch[2] = 0;
                SYNMidiInput(&sequence->synth, ch);
                OSRestoreInterrupts(old);
            }
            break;
        }
    }
    sequence->state = state;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
