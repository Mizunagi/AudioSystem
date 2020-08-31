#include "CBiquadIIRFilter.h"

CBiquadIIRFilter::CBiquadIIRFilter() {
}

CBiquadIIRFilter::~CBiquadIIRFilter() {
}

float CBiquadIIRFilter::IIRFiltering(float val) {
	float	dest =
		m_Coefficients_b[0] / m_Coefficients_a[0] * val +
		m_Coefficients_b[1] / m_Coefficients_a[0] * m_SrcBuf[0] +
		m_Coefficients_b[2] / m_Coefficients_a[0] * m_SrcBuf[1] -
		m_Coefficients_a[1] / m_Coefficients_a[0] * m_DestBuf[0] -
		m_Coefficients_a[2] / m_Coefficients_a[0] * m_DestBuf[1];

	m_SrcBuf[1] = m_SrcBuf[0];
	m_SrcBuf[0] = val;

	m_DestBuf[1] = m_DestBuf[0];
	m_DestBuf[0] = dest;
	return dest;
}

void CBiquadIIRFilter::LowPass(float cutoffFreq, float q, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha;

	m_Coefficients_b[0] = (1.0f - cos_Omega) / 2.0f;
	m_Coefficients_b[1] = 1.0f - cos_Omega;
	m_Coefficients_b[2] = (1.0f - cos_Omega) / 2.0f;
}

void CBiquadIIRFilter::HighPass(float cutoffFreq, float q, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha;

	m_Coefficients_b[0] = (1.0f + cos_Omega) / 2.0f;
	m_Coefficients_b[1] = -(1.0f + cos_Omega);
	m_Coefficients_b[2] = (1.0f + cos_Omega) / 2.0f;
}

void CBiquadIIRFilter::BandPass(float centerFreq, float bandwidth, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float cos_Omega = cos(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha;

	m_Coefficients_b[0] = Alpha;
	m_Coefficients_b[1] = 0.0f;
	m_Coefficients_b[2] = -Alpha;
}

void CBiquadIIRFilter::BandStop(float centerFreq, float bandwidth, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float cos_Omega = cos(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha;

	m_Coefficients_b[0] = Alpha;
	m_Coefficients_b[1] = 0.0f;
	m_Coefficients_b[2] = -Alpha;
}

void CBiquadIIRFilter::LowShelf(float cutoffFreq, float q, float gain, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float Amplification = pow(10.0f, (gain / 40.0f));
	const float Beta = sqrt(Amplification) / q;

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	m_Coefficients_a[0] = (Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega;
	m_Coefficients_a[1] = -2.0f * ((Amplification - 1.0f) + (Amplification + 1.0f) * cos_Omega);
	m_Coefficients_a[2] = (Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega;

	m_Coefficients_b[0] = Amplification * ((Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega);
	m_Coefficients_b[1] = 2.0f * Amplification * ((Amplification - 1.0f) - (Amplification + 1.0f) * cos_Omega);
	m_Coefficients_b[2] = Amplification * (Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega;
}

void CBiquadIIRFilter::HighShelf(float cutoffFreq, float q, float gain, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * cutoffFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float Amplification = pow(10.0f, (gain / 40.0f));
	const float Beta = sqrt(Amplification) / q;

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	m_Coefficients_a[0] = (Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega;
	m_Coefficients_a[1] = 2.0f * ((Amplification - 1.0f) - (Amplification + 1.0f) * cos_Omega);
	m_Coefficients_a[2] = (Amplification + 1.0f) - (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega;

	m_Coefficients_b[0] = Amplification * ((Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega + Beta * sin_Omega);
	m_Coefficients_b[1] = -2.0f * Amplification * ((Amplification - 1.0f) + (Amplification + 1.0f) * cos_Omega);
	m_Coefficients_b[2] = Amplification * (Amplification + 1.0f) + (Amplification - 1.0f) * cos_Omega - Beta * sin_Omega;
}

void CBiquadIIRFilter::Peaking(float centerFreq, float bandwidth, float gain, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) * sinh(log(2.0f) / 2.0f * bandwidth * Omega / sin(Omega));

	const float Amplification = pow(10.0f, (gain / 40.0f));

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha / Amplification;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha / Amplification;

	m_Coefficients_b[0] = 1.0f + Alpha * Amplification;
	m_Coefficients_b[1] = -2.0f * cos_Omega;
	m_Coefficients_b[2] = 1.0f - Alpha * Amplification;
}

void CBiquadIIRFilter::AllPass(float centerFreq, float q, uint32_t samplingRate) {
	if (samplingRate == 0)
		return;

	const float Omega = 2.0f * static_cast<float>(M_PI) * centerFreq / samplingRate;
	const float Alpha = sin(Omega) / (2.0f * q);

	const float cos_Omega = cos(Omega);
	const float sin_Omega = sin(Omega);

	m_Coefficients_a[0] = 1.0f + Alpha;
	m_Coefficients_a[1] = -2.0f * cos_Omega;
	m_Coefficients_a[2] = 1.0f - Alpha;

	m_Coefficients_b[0] = 1.0f - Alpha;
	m_Coefficients_b[1] = -2.0f * cos_Omega;
	m_Coefficients_b[2] = 1.0f + Alpha;
}