#pragma once
#include "AudioDefine.h"

#define EQ_COEFICIENTS 3
#define EQ_BUFFERS 2

class CBiquadIIRFilter {
public:
	CBiquadIIRFilter();
	~CBiquadIIRFilter();

	float IIRFiltering(float val);
	void Flush() {
		for (uint16_t i = 0; i < EQ_BUFFERS; ++i) {
			m_SrcBuf[i] = m_DestBuf[i] = 0.0f;
		}
	}

	void	LowPass(float	cutoffFreq, float	q, uint32_t	samplingRate);
	void	HighPass(float	cutoffFreq, float	q, uint32_t	samplingRate);
	void	BandPass(float	centerFreq, float	bandwidth, uint32_t	samplingRate);
	void	BandStop(float	centerFreq, float	bandwidth, uint32_t	samplingRate);
	void	LowShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate);
	void	HighShelf(float	cutoffFreq, float	q, float	gain, uint32_t	samplingRate);
	void	Peaking(float	centerFreq, float	bandwidth, float	gain, uint32_t	samplingRate);
	void	AllPass(float	centerFreq, float	q, uint32_t	samplingRate);
private:
	float m_Coefficients_a[EQ_COEFICIENTS]{ 0.0f,0.0f,0.0f }, m_Coefficients_b[EQ_COEFICIENTS]{ 0.0f,0.0f,0.0f };
	float m_SrcBuf[EQ_BUFFERS]{ 0.0f,0.0f }, m_DestBuf[EQ_BUFFERS]{ 0.0f,0.0f };
};
