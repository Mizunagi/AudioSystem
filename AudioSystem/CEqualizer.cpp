#include "CEqualizer.h"

CEqualizer::CEqualizer(AudioFormat _foramt) :CEffectBase(_foramt) {
}

CEqualizer::~CEqualizer() {
}

void CEqualizer::SetEffectParam(EffectParam& _param) {
	std::lock_guard lock(m_ParamMutex);
	EQParam param = static_cast<EQParam&>(_param);

	if (m_Filters.size() != param.params.size()) {
		m_Filters.resize(param.params.size());
	}

	for (uint16_t cntParam = 0; auto & fil : m_Filters) {
		auto oneParam = param.params[cntParam];
		switch (oneParam._type) {
		case EQType::EQTYPE_LOWPASS:
			fil._coefficients = LowPass(oneParam._freq, oneParam._q, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_HIGHPASS:
			fil._coefficients = HighPass(oneParam._freq, oneParam._q, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_BANDPASS:
			fil._coefficients = BandPass(oneParam._freq, oneParam._bandwidth, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_BANDSTOP:
			fil._coefficients = Notch(oneParam._freq, oneParam._bandwidth, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_ALLPASS:
			fil._coefficients = AllPass(oneParam._freq, oneParam._q, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_LOWSHELF:
			fil._coefficients = LowShelf(oneParam._freq, oneParam._q, oneParam._gain, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_HIGHSHELF:
			fil._coefficients = HighShelf(oneParam._freq, oneParam._q, oneParam._gain, m_Format.samplingrate);
			break;
		case EQType::EQTYPE_PEAKING:
			fil._coefficients = Peaking(oneParam._freq, oneParam._bandwidth, oneParam._gain, m_Format.samplingrate);
			break;
		}
		fil._buffers.resize(m_Format.channel);
		cntParam++;
	}
}

IIRCoefficients	CEqualizer::LowPass(float	cutoffFreq, float	q, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);

	return IIRCoefficients(
		1.0f + Alpha,
		-2.0f * cos_Omega,
		1.0f - Alpha,
		(1.0f - cos_Omega) / 2.0f,
		1.0f - cos_Omega,
		(1.0f - cos_Omega) / 2.0f
	);
}

IIRCoefficients	CEqualizer::HighPass(float	cutoffFreq, float	q, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);

	return IIRCoefficients(
		1.0f + Alpha,
		-2.0f * cos_Omega,
		1.0f - Alpha,
		(1.0f + cos_Omega) / 2.0f,
		-(1.0f + cos_Omega),
		(1.0f + cos_Omega) / 2.0f
	);
}

IIRCoefficients	CEqualizer::BandPass(float	centerFreq, float	bandwidth, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float cos_Omega = cos(Omega);

	return IIRCoefficients(
		1.0f + Alpha,
		-2.0f * cos_Omega,
		1.0f - Alpha,
		Alpha,
		0.0f,
		-Alpha
	);
}

IIRCoefficients	CEqualizer::Notch(float	centerFreq, float	bandwidth, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float cos_Omega = cos(Omega);

	return IIRCoefficients(
		1.0f + Alpha,
		-2.0f * cos_Omega,
		1.0f - Alpha,
		1.0f,
		-2.0f * cos_Omega,
		1.0f
	);
}

IIRCoefficients	CEqualizer::LowShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float Amplification = pow(10.0f, (gain / 40.0f));
	const float Beta = sqrt(Amplification) / q;

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	return IIRCoefficients(
		(Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega,
		-2.0f * ((Amplification - 1.0f) + (Amplification + 1.0f) * cos_Omega),
		(Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega,
		Amplification * ((Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega),
		2.0f * Amplification * ((Amplification - 1.0f) - (Amplification + 1.0f) * cos_Omega),
		Amplification * (Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega
	);
}

IIRCoefficients	CEqualizer::HighShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float Amplification = pow(10.0f, (gain / 40.0f));
	const float Beta = sqrt(Amplification) / q;

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	return IIRCoefficients(
		(Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega,
		2.0f * ((Amplification - 1.0f) - (Amplification + 1.0f) * cos_Omega),
		(Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega,
		Amplification * ((Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega),
		-2.0f * Amplification * ((Amplification - 1.0f) + (Amplification + 1.0f) * cos_Omega),
		Amplification * (Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega
	);
}

IIRCoefficients	CEqualizer::Peaking(float	centerFreq, float	bandwidth, float	gain, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float Amplification = pow(10.0f, (gain / 40.0f));

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	return IIRCoefficients(
		1.0f + Alpha / Amplification,
		-2.0f * cos_Omega,
		1.0f - Alpha / Amplification,
		1.0f + Alpha * Amplification,
		-2.0f * cos_Omega,
		1.0f - Alpha * Amplification
	);
}

IIRCoefficients	CEqualizer::AllPass(float	centerFreq, float	q, uint32_t	samplingRate) {
	if (samplingRate == 0)
		return IIRCoefficients(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	return IIRCoefficients(
		1.0f + Alpha,
		-2.0f * cos_Omega,
		1.0f - Alpha,
		1.0f - Alpha,
		-2.0f * cos_Omega,
		1.0f + Alpha
	);
}

void CEqualizer::Process(CLineBuffer<float>& _buffer, uint32_t _rednerFrames) {
	std::lock_guard lock(m_ParamMutex);
	for (uint16_t chan = 0; chan < m_Format.channel; ++chan) {
		for (uint32_t frame = 0; frame < _rednerFrames; ++frame) {
			float temp = _buffer[chan][frame];
			for (auto& fil : m_Filters) {
				auto coef = fil._coefficients;
				auto filBuf = fil._buffers[chan];

				float dest =
					coef._beta[0] / coef._alpha[0] * temp +
					coef._beta[1] / coef._alpha[0] * filBuf._src[0] +
					coef._beta[2] / coef._alpha[0] * filBuf._src[1] -
					coef._alpha[1] / coef._alpha[0] * filBuf._dest[0] -
					coef._alpha[2] / coef._alpha[0] * filBuf._dest[1];

				filBuf._src[1] = filBuf._src[0];
				filBuf._src[0] = temp;

				filBuf._dest[1] = filBuf._dest[0];
				filBuf._dest[0] = dest;

				temp = dest;
			}
			if (Enable())
				_buffer[chan][frame] = temp;
		}
	}
}

void CEqualizer::Flush() {
	for (auto& fil : m_Filters) {
		fil.Flush();
	}
}