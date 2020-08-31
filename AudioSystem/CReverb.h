#pragma once
#include "CEffectBase.h"

struct ReverbParam :public EffectParam {
	ReverbParam() {}
	ReverbParam(float _delayTime, float _decayLevel, float _feedBack)
		: delayTime(_delayTime), decayLevel(_decayLevel), feedBack(_feedBack)
	{
	}
	float delayTime = 0.0f;		//50~2000mm
	float decayLevel = 0.0f;	//0~1
	float feedBack = 0.0f;		//0~1
};

class CReverb : public CEffectBase {
public:
	CReverb(AudioFormat _format);
	~CReverb();

	void SetEffectParam(EffectParam& _param) override;
private:

	ReverbParam m_Param;
	uint32_t m_DelayPoint = 0;
	std::vector<CCircularBuffer<float>> m_CircularBufs;

	void Process(CLineBuffer<float>& _buffer, uint32_t _rednerFrames)override;
	void Flush()override;
};
