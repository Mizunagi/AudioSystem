#pragma once
#include "CEffectBase.h"
#include "CBiquadIIRFilter.h"

#define EQ_COEFICIENTS 3
#define EQ_BUFFERS 2

enum class EQType {
	EQTYPE_LOWPASS,		//���[�p�X�t�B���^
	EQTYPE_HIGHPASS,	//�n�C�p�X�t�B���^
	EQTYPE_BANDPASS,	//�o���h�p�X�t�B���^
	EQTYPE_BANDSTOP,	//�o���h�X�g�b�v�t�B���^
	EQTYPE_ALLPASS,		//�I�[���p�X�t�B���^
	EQTYPE_LOWSHELF,	//���[�V�F���t�t�B���^
	EQTYPE_HIGHSHELF,	//�n�C�V�F���t�t�B���^
	EQTYPE_PEAKING,		//�s�[�L���O�t�B���^
};

struct EQSingleParam {
	float	_freq;		//�C�R���C�W���O���g��
	float	_q;			//Q�l
	float	_bandwidth;	//�o���h��
	float	_gain;		//�Q�C���l
	EQType	_type;		//�C�R���C�W���O�^�C�v
	EQSingleParam() {
		_freq = 1200.0f;
		_q = 1.0f;
		_bandwidth = 1.0f;
		_gain = 1.0f;
		_type = EQType::EQTYPE_PEAKING;
	}
	EQSingleParam(float freq, float q, float bandwidth, float gain, EQType type) {
		_freq = freq;
		_q = q;
		_bandwidth = bandwidth;
		_gain = gain;
		_type = type;
	}
};

struct EQParam :public EffectParam {
	EQParam() {}
	EQParam(std::vector<EQSingleParam> _params) :params(_params) {}
	std::vector<EQSingleParam> params{};
};

struct IIRBuffer {
	std::array<float, EQ_BUFFERS> _src{ 0.0f,0.0f };
	std::array<float, EQ_BUFFERS> _dest{ 0.0f,0.0f };
};

struct IIRCoefficients {
	std::array<float, EQ_COEFICIENTS> _alpha{ 0.0f,0.0f,0.0f };
	std::array<float, EQ_COEFICIENTS> _beta{ 0.0f,0.0f ,0.0f };

	IIRCoefficients() {}
	IIRCoefficients(float _ca0, float _ca1, float _ca2, float _cb0, float _cb1, float _cb2) {
		_alpha = std::array<float, EQ_COEFICIENTS>{_ca0, _ca1, _ca2};
		_beta = std::array<float, EQ_COEFICIENTS>{_cb0, _cb1, _cb2};
	};
};

struct IIRFilter {
	std::vector<IIRBuffer> _buffers;
	IIRCoefficients _coefficients;
	void Flush() {
		for (auto& buf : _buffers) {
			buf._src = buf._dest = std::array<float, EQ_BUFFERS>{0.0f, 0.0f};
		}
	}
};

//=============================================================================
//!	@class	:CEqualizer
//!	@brief	:�C�R���C�U
//=============================================================================
class CEqualizer : public CEffectBase {
public:
	CEqualizer(AudioFormat _format);
	~CEqualizer();
	void SetEffectParam(EffectParam& _param) override;
private:

	IIRCoefficients	LowPass(float	cutoffFreq, float	q, uint32_t	samplingRate);
	IIRCoefficients	HighPass(float	cutoffFreq, float	q, uint32_t	samplingRate);
	IIRCoefficients	BandPass(float	centerFreq, float	bandwidth, uint32_t	samplingRate);
	IIRCoefficients	Notch(float	centerFreq, float	bandwidth, uint32_t	samplingRate);
	IIRCoefficients	LowShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate);
	IIRCoefficients	HighShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate);
	IIRCoefficients	Peaking(float	centerFreq, float	bandwidth, float	gain, uint32_t	samplingRate);
	IIRCoefficients	AllPass(float	centerFreq, float	q, uint32_t	samplingRate);

	std::vector<IIRFilter> m_Filters;

	void Process(CLineBuffer<float>& _buffer, uint32_t _rednerFrames) override;
	void Flush()override;
};
