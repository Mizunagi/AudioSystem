#include "CWaveBase.h"

CWaveBase::CWaveBase() {
}

CWaveBase::~CWaveBase() {
	if (m_WaveStream.is_open()) {
		m_WaveStream.close();
	}
}

uint32_t CWaveBase::GetBuffer(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames) {
	switch (m_HoldMode)
	{
	case EHoldMode::WAVE_HOLDMODE_NONE:
		return 0;
	case EHoldMode::WAVE_HOLDMODE_LOADALL:
		return GetMemory(_dest, _destPoint, _srcPoint, _frames);
	case EHoldMode::WAVE_HOLDMODE_STREAM:
		return GetStream(_dest, _destPoint, _srcPoint, _frames);
	default:
		return 0;
	}
}

uint32_t CWaveBase::GetMemory(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames) {
	for (uint32_t i = 0; i < m_WaveInfo.channel; ++i) {
		std::memcpy(&_dest[i][_destPoint], &m_Wave[i][_srcPoint], sizeof(float) * _frames);
	}
	return _frames;
}