#include <dolphin/types.h>

#define THP_ACCUM_FRAC_BITS 16
#define THP_COEFF_FRAC_BITS 11
#define THP_RESCALE_BITS (THP_ACCUM_FRAC_BITS - THP_COEFF_FRAC_BITS)
#define THP_PCM_FRAC_MASK ((1U << THP_ACCUM_FRAC_BITS) - 1)
#define THP_PCM_ROUND_HALF (1U << (THP_ACCUM_FRAC_BITS - 1))
#define THP_PCM_ROUND_UNIT (1U << THP_ACCUM_FRAC_BITS)
#define THP_ACCUM_MIN (-1LL - INT_MAX)

#define THP_PCM_ROUND_AND_CLAMP(work)                                 \
    do {                                                              \
        if ((u16)((work) & THP_PCM_FRAC_MASK) > THP_PCM_ROUND_HALF) {  \
            (work) += THP_PCM_ROUND_UNIT;                             \
        } else if ((u16)((work) & THP_PCM_FRAC_MASK) == THP_PCM_ROUND_HALF && \
                   ((work) & THP_PCM_ROUND_UNIT)) {                   \
            (work) += THP_PCM_ROUND_UNIT;                             \
        }                                                             \
        if ((work) > INT_MAX) {                                       \
            (work) = INT_MAX;                                         \
        }                                                             \
        if ((work) < THP_ACCUM_MIN) {                                 \
            (work) = THP_ACCUM_MIN;                                   \
        }                                                             \
    } while (0)

#define THP_ADPCM_BLOCK_NIBBLES 16
#define THP_ADPCM_HEADER_NIBBLES 2
#define THP_NIBBLE_BITS 4
#define THP_NIBBLE_MASK ((1U << THP_NIBBLE_BITS) - 1U)
#define THP_PREDICTOR_MASK 0x7
#define THP_NIBBLE_SIGN_SHIFT (32 - THP_NIBBLE_BITS)
#define THP_HIGH_NIBBLE_SHIFT (32 - 2 * THP_NIBBLE_BITS)

typedef struct _THPAudioDecodeStruct {
    u8* p;
    u32 nibbleIndex;
    u8 predictor;
    u8 scale;
    u8 opaque[6];
} THPAudioDecodeStruct;

typedef struct _THPSample {
    u32 channelSize;
    u32 numSamples;
    s16 coeff[2][8][2];
    s16 prev[2][2];
} THPSample;

static s32 __THPAudioGetNewSample(THPAudioDecodeStruct* audio);
static void __THPAudioInitialize(THPAudioDecodeStruct* audio, u8* data);

u32 THPAudioDecode(s16* out, u8* frame, s32 planar)
{
    THPAudioDecodeStruct audio;
    THPSample* sample;
    s16 value;
    s64 work;
    s16 hist0;
    u8* leftData;
    u8* rightData;
    u32 stride;
    s16* rightOut;
    s16 hist1;
    s16* leftOut;
    u32 i;

    if (out == 0 || frame == 0) {
        return 0;
    }

    sample = (THPSample*)frame;
    leftData = frame + sizeof(*sample);
    rightData = leftData + sample->channelSize;
    if (planar == TRUE) {
        rightOut = out;
        stride = 1;
        leftOut = out + sample->numSamples;
    } else {
        rightOut = out;
        leftOut = out + 1;
        stride = 2;
    }

    if (sample->channelSize == 0) {
        __THPAudioInitialize(&audio, leftData);
        hist0 = sample->prev[0][0];
        hist1 = sample->prev[0][1];
        for (i = 0; i < sample->numSamples; i++) {
            s32 nibble = __THPAudioGetNewSample(&audio);

            work = sample->coeff[0][audio.predictor][1] * hist1;
            work += sample->coeff[0][audio.predictor][0] * hist0;
            work += (s32)(((u32)nibble << audio.scale) << THP_COEFF_FRAC_BITS);
            work <<= THP_RESCALE_BITS;
            THP_PCM_ROUND_AND_CLAMP(work);
            value = (*leftOut = work >> THP_ACCUM_FRAC_BITS);
            hist1 = hist0;
            hist0 = value;
            leftOut += stride;
            *rightOut = value;
            rightOut += stride;
        }
    } else {
        __THPAudioInitialize(&audio, leftData);
        hist0 = sample->prev[0][0];
        hist1 = sample->prev[0][1];
        for (i = 0; i < sample->numSamples; i++) {
            s32 nibble = __THPAudioGetNewSample(&audio);

            work = sample->coeff[0][audio.predictor][1] * hist1;
            work += sample->coeff[0][audio.predictor][0] * hist0;
            work += (s32)(((u32)nibble << audio.scale) << THP_COEFF_FRAC_BITS);
            work <<= THP_RESCALE_BITS;
            THP_PCM_ROUND_AND_CLAMP(work);
            value = work >> THP_ACCUM_FRAC_BITS;

            *leftOut = value;
            hist1 = hist0;
            hist0 = value;
            leftOut += stride;
        }

        __THPAudioInitialize(&audio, rightData);
        hist0 = sample->prev[1][0];
        hist1 = sample->prev[1][1];
        for (i = 0; i < sample->numSamples; i++) {
            s32 nibble = __THPAudioGetNewSample(&audio);

            work = sample->coeff[1][audio.predictor][1] * hist1;
            work += sample->coeff[1][audio.predictor][0] * hist0;
            work += (s32)(((u32)nibble << audio.scale) << THP_COEFF_FRAC_BITS);
            work <<= THP_RESCALE_BITS;
            THP_PCM_ROUND_AND_CLAMP(work);
            value = work >> THP_ACCUM_FRAC_BITS;

            *rightOut = value;
            hist1 = hist0;
            hist0 = value;
            rightOut += stride;
        }
    }

    return sample->numSamples;
}

static s32 __THPAudioGetNewSample(THPAudioDecodeStruct* audio)
{
    s32 result;

    if ((audio->nibbleIndex & (THP_ADPCM_BLOCK_NIBBLES - 1)) == 0) {
        audio->predictor = ((audio->p[0] >> THP_NIBBLE_BITS) & THP_PREDICTOR_MASK);
        audio->scale = audio->p[0] & THP_NIBBLE_MASK;
        audio->p++;
        audio->nibbleIndex += THP_ADPCM_HEADER_NIBBLES;
    }

    if (audio->nibbleIndex & 1) {
        u32 nibble = *audio->p & THP_NIBBLE_MASK;
        audio->p++;
        result = (s32)(nibble << THP_NIBBLE_SIGN_SHIFT) >> THP_NIBBLE_SIGN_SHIFT;
    } else {
        u32 data = *audio->p;
        result = (s32)((data << THP_HIGH_NIBBLE_SHIFT) & (THP_NIBBLE_MASK << THP_NIBBLE_SIGN_SHIFT)) >> THP_NIBBLE_SIGN_SHIFT;
    }
    audio->nibbleIndex++;
    return result;
}

static void __THPAudioInitialize(THPAudioDecodeStruct* audio, u8* data)
{
    audio->p = data;
    audio->nibbleIndex = THP_ADPCM_HEADER_NIBBLES;
    audio->predictor = (audio->p[0] >> THP_NIBBLE_BITS) & THP_PREDICTOR_MASK;
    audio->scale = audio->p[0] & THP_NIBBLE_MASK;
    audio->p++;
}
