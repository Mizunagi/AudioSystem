#include "CReverb.h"

CReverb::CReverb(AudioFormat _format) :CEffectBase(_format) {
}

CReverb::~CReverb() {
}

void CReverb::SetEffectParam(EffectParam& _param) {
	std::lock_guard lock(m_ParamMutex);
	m_Param = static_cast<ReverbParam&>(_param);

	//Create CircularBuffer
	std::vector<CCircularBuffer<float>> circularBuf(m_Format.channel);
	m_DelayPoint = static_cast<uint32_t>(m_Format.samplingrate * m_Param.delayTime / 1000.0f);
	for (auto& cir : circularBuf) {
		cir.resize(static_cast<size_t>(m_DelayPoint) + (m_DelayPoint % 32));
	}
	m_CircularBufs = std::move(circularBuf);
}

void CReverb::Process(CLineBuffer<float>& _buffer, uint32_t _rednerFrames) {
	std::lock_guard lock(m_ParamMutex);
	for (uint32_t fram = 0; fram < _rednerFrames; ++fram) {
		for (uint32_t chan = 0; auto & cir : m_CircularBufs) {
			float src = _buffer[chan][fram];
			//入力信号にディレイ信号を加える
			float dest = src + (m_Param.decayLevel * cir[0]);
			//ディレイ信号として入力信号と今回のディレイ信号をバッファへ書込み
			cir[m_DelayPoint] = src + (m_Param.feedBack * cir[0]);
			//Output(循環バッファは更新しないとリバーブが狂う)
			_buffer[chan][fram] = Enable() ? dest : src;
			///todo ユーザーにenable()をさせるより引数でenableを用意した方がいいのでは？
			//Move buffer
			++cir;
			++chan;
		}
	}
}

void CReverb::Flush() {
	for (auto& cir : m_CircularBufs) {
		cir.clear();
	}
}