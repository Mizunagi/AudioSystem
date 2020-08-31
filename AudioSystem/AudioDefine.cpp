#include "AudioDefine.h"

void PCM_Normalize(byte* pSrc, CLineBuffer<float>& _dest, uint32_t bitDepth, uint32_t channels, uint32_t frames) {
	void(*normalizeFunc)(byte*, float*) = nullptr;
	uint16_t bdSize = bitDepth / BITPERBYTE;
	switch (bitDepth) {
	case 16:
		normalizeFunc = PCM_Normalize16bit;
		break;
	}

	if (!normalizeFunc)return;

	uint32_t loadPoint = 0;
	for (uint64_t fram = 0; fram < frames; ++fram) {
		for (uint16_t chan = 0; chan < channels; ++chan) {
			normalizeFunc(&pSrc[loadPoint], &_dest[chan][fram]);
			loadPoint += bdSize;
		}
	}
}

void PCM_Denormalize(CLineBuffer<float>& ppSrc, byte* pDest, uint32_t bitDepth, uint32_t channels, uint32_t frames) {
	void(*denormalizeFunc)(float*, byte*) = nullptr;
	uint16_t bdSize = bitDepth / BITPERBYTE;
	switch (bitDepth) {
	case 16:
		denormalizeFunc = PCM_Denormalize16bit;
		break;
	}

	if (!denormalizeFunc)return;

	uint32_t writePoint = 0;
	for (uint64_t fram = 0; fram < frames; ++fram) {
		for (uint16_t chan = 0; chan < channels; ++chan) {
			denormalizeFunc(&ppSrc[chan][fram], &pDest[writePoint]);
			writePoint += bdSize;
		}
	}
}

void PCM_Normalize16bit(byte* pSrc, float* pDest) {
	int16_t raw = 0;
	memcpy(&raw, pSrc, 2);
	*pDest = static_cast<float>(raw) / static_cast<float>(0x8000);
}

void PCM_Denormalize16bit(float* pSrc, byte* pDest) {
	float src = *pSrc;
	src += 1.0f;
	src = src / 2.0f * static_cast<float>(0x10000);
	if (src > static_cast<float>(0xffff))
		src = static_cast<float>(0xffff);
	else if (src < 0.0f)
		src = 0.0f;

	int16_t data = static_cast<int16_t>(static_cast<int32_t>(src + 0.5) - static_cast<int32_t>(0x8000));
	memcpy(pDest, &data, 2);
}