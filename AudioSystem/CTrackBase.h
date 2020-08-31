#pragma once
#include "AudioDefine.h"
#include "CStdPtrHelper.h"
#include "CEffectManager.h"

class CTrackBase;
using SPTrackBase = std::shared_ptr<CTrackBase>;
using WPTrackBase = std::weak_ptr<CTrackBase>;

class CTrackBase :public std::enable_shared_from_this<CTrackBase> {
public:
	friend class CAudioSystem;
	friend class CMasterTrack;

	virtual ~CTrackBase() {}

	float Volume(float _vol) {
		return m_Volume = VOLUME_MAX >= _vol && _vol >= VOLUME_MIN ? _vol :
			_vol > VOLUME_MAX ? VOLUME_MAX :
			_vol < VOLUME_MIN ? VOLUME_MIN :
			0.0f;
	}
	float Volume() { return m_Volume; }
	AudioFormat	GetFormat() { return m_Format; }

	virtual size_t GetBuffer(CLineBuffer<float>& _buffer, uint32_t _frames) = 0;
	virtual size_t ConnectTrack(const std::shared_ptr<CTrackBase> child) = 0;
protected:
	CTrackBase() {};

	AudioFormat m_Format;
	float m_Volume = 0.5f;
	uint32_t m_Frames = 0;
	std::shared_ptr<CEffectManager> m_Effector;
};
