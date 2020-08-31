#pragma once
#include "AudioDefine.h"
#include "CStdPtrHelper.h"

enum class EHoldMode {
	WAVE_HOLDMODE_NONE = 0,
	WAVE_HOLDMODE_LOADALL,
	WAVE_HOLDMODE_STREAM
};

class CWaveBase {
public:
	uint32_t GetBuffer(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames);

	const CLineBuffer<float>& GetTrack() { return m_Wave; }
	const AudioInfo GetInfo() { return m_WaveInfo; }
protected:
	CWaveBase();
	virtual ~CWaveBase();

	virtual uint32_t GetStream(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames) = 0;
	uint32_t GetMemory(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames);

	AudioInfo m_WaveInfo;
	EHoldMode m_HoldMode = EHoldMode::WAVE_HOLDMODE_NONE;

	CLineBuffer<float> m_Wave;

	std::fstream m_WaveStream;
	uint32_t m_StreamPoint = 0;
};

using SPWaveBase = std::shared_ptr<CWaveBase>;