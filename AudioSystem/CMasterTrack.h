#pragma once
#include "CTrackBase.h"

class CMasterTrack;
using SPMasterTrack = std::shared_ptr<CMasterTrack>;
using WPMasterTrack = std::weak_ptr <CMasterTrack>;

class CMasterTrack :public CTrackBase {
public:
	friend class CAudioSystem;
	template<typename T>
	friend class CStdPtrHelper;

	const size_t GetSourceSize() { return m_Children.size(); }
private:
	CMasterTrack(AudioFormat& _format, const uint32_t _masterFrames);
	~CMasterTrack();

	static std::shared_ptr<CMasterTrack> CreateInstance(AudioFormat& _format, uint32_t _createFrames) { return CStdPtrHelper<CMasterTrack>::make_shared(_format, _createFrames); }

	size_t GetBuffer(CLineBuffer<float>& _buffer, uint32_t _frames)override;
	size_t ConnectTrack(const std::shared_ptr<CTrackBase> child)override;

	CLineBuffer<float> m_MixTrack;
	std::vector<std::weak_ptr<CTrackBase>> m_Children;
};
