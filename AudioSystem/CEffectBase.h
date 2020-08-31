#pragma once
#include "AudioDefine.h"
#include "CStdPtrHelper.h"

class CEffectBase;
using SPEffectBase = std::shared_ptr<CEffectBase>;
using WPEffectBase = std::weak_ptr<CEffectBase>;

struct EffectParam {};

class CEffectBase {
public:
	friend class CEffectManager;

	CEffectBase(AudioFormat _format) { m_Format = _format; }
	virtual ~CEffectBase() {};

	virtual void SetEffectParam(EffectParam& _param) = 0;
	void Enable(bool _enable) { m_bEnable = m_bEnable != _enable ? _enable : m_bEnable; }
	const bool Enable() { return m_bEnable; }
protected:
	std::mutex m_ParamMutex;
	AudioFormat m_Format;

	virtual void Process(CLineBuffer<float>& _buffer, uint32_t _renderFrames) = 0;
	virtual void Flush() = 0;

private:
	bool m_bEnable = true;
};
