#include "CMasterTrack.h"

CMasterTrack::CMasterTrack(AudioFormat& _format, const uint32_t _masterFrames) {
	m_Format = _format;
	m_MixTrack.resize(m_Format.channel, _masterFrames);
	std::stringstream logstr;
	logstr << "Create MasterTrack\n"
		<< "Create Format\n"
		<< "\tChannels\t:\t" << m_Format.channel << "\n"
		<< "\tSamplingRate\t:\t" << m_Format.samplingrate << "\n"
		<< "\tBitDepth\t:\t" << m_Format.bitdepth << "\n"
		<< "Created Master Buffer Frame\t:\t" << _masterFrames;
	CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CMasterTrack).name()), __func__, "", logstr.str());
}

CMasterTrack::~CMasterTrack() {
	//RemoveEffect();
}

size_t CMasterTrack::GetBuffer(CLineBuffer<float>& _buffer, uint32_t _frames) {
	std::erase_if(m_Children, [](auto child) {return child.expired(); });//erase expried weak reference

	for (auto& child : m_Children) {
		if (auto source = child.lock()) {
			auto sourceFormat = source->GetFormat();
			if (m_Format.samplingrate != sourceFormat.samplingrate || m_Format.bitdepth != sourceFormat.bitdepth || m_Format.channel != sourceFormat.channel)
				continue;

			m_MixTrack.zeroclear();
			source->GetBuffer(m_MixTrack, _frames);
			const uint32_t add = sizeof(__m256) / sizeof(float);
			for (uint32_t chan = 0; chan < m_Format.channel; ++chan) {
				for (uint32_t fram = 0; fram < m_MixTrack.sizeRealX(); fram += add) {
					__m256 a = _mm256_load_ps(&m_MixTrack[chan][fram]);
					__m256 b = _mm256_load_ps(&_buffer[chan][fram]);
					__m256 res = _mm256_add_ps(a, b);
					_mm256_store_ps(&_buffer[chan][fram], res);
				}
			}
		}
	}
	return _frames;
}

size_t CMasterTrack::ConnectTrack(const std::shared_ptr<CTrackBase> child) {
	if (typeid(child) == typeid(CMasterTrack))return m_Children.size();
	auto itr = std::find_if(m_Children.begin(), m_Children.end(), [&child](auto children) {return child == children.lock(); });
	if (itr == m_Children.end()) {
		m_Children.push_back(child);
	}
	return m_Children.size();
}