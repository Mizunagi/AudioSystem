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
			//���͐M���Ƀf�B���C�M����������
			float dest = src + (m_Param.decayLevel * cir[0]);
			//�f�B���C�M���Ƃ��ē��͐M���ƍ���̃f�B���C�M�����o�b�t�@�֏�����
			cir[m_DelayPoint] = src + (m_Param.feedBack * cir[0]);
			//Output(�z�o�b�t�@�͍X�V���Ȃ��ƃ��o�[�u������)
			_buffer[chan][fram] = Enable() ? dest : src;
			///todo ���[�U�[��enable()���������������enable��p�ӂ������������̂ł́H
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