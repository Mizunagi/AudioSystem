#pragma once
#include "Utilities.h"
#include "CSingleton.h"
#include "CLineBuffer.h"
#include "CCircularBuffer.h"
#include "CLog.h"

#define _TEST_ false

#define BITPERBYTE 8
#define VOLUME_MAX 1.0f
#define VOLUME_MIN 0.0f
#define LOOP_INFINITY -1
#define NOPADDING -1
#define PROCESSERROR -1

using byte = uint8_t;

struct AudioFormat {
	AudioFormat() :samplingrate(0), bitdepth(0), channel(0) {}
	AudioFormat(uint32_t _sampling, uint32_t _depth, uint32_t _channel) :samplingrate(_sampling), bitdepth(_depth), channel(_channel) {}
	bool operator ==(const AudioFormat _format) {
		if (samplingrate != _format.samplingrate)return false;
		if (bitdepth != _format.bitdepth)return false;
		if (channel != _format.channel)return false;
		return true;
	}
	uint32_t samplingrate;
	uint32_t bitdepth;
	uint32_t channel;
};

struct AudioInfo :public AudioFormat {
	AudioInfo() :name(""), AudioFormat(), allFrames(0) {}
	AudioInfo(std::string _name, uint32_t _sampling, uint32_t _depth, uint32_t _channel, uint32_t _frames) :name(_name), allFrames(_frames), AudioFormat(_sampling, _depth, _channel) {}
	AudioInfo(std::string _name, AudioFormat& _format, uint32_t _frames) :name(_name), allFrames(_frames), AudioFormat(_format) {}
	const AudioFormat Format() { return AudioFormat(samplingrate, bitdepth, channel); }
	std::string name;
	uint32_t allFrames;
};

void PCM_Normalize(byte* pSrc, CLineBuffer<float>& _dest, uint32_t _bitDepth, uint32_t _channels, uint32_t _frames);
void PCM_Denormalize(CLineBuffer<float>& _src, byte* _pDest, uint32_t _bitDepth, uint32_t _channels, uint32_t _frames);
void PCM_Normalize16bit(byte* pSrc, float* pDest);
void PCM_Denormalize16bit(float* pSrc, byte* pDest);