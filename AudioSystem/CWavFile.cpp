#include "CWavFile.h"

bool CWavFile::Load(std::string _directory, EHoldMode _mode) {
	std::fstream stream(_directory, std::ios::in | std::ios::binary);
	if (!stream.is_open()) {
		CLog::Log(CLog::ELogLevel::ASLOG_ERROR, _directory + " Failed open file");
		return false;
	}

	uint32_t wavPoint = 0;

	RIFFChunk riff{};
	FMTChunk fmt{};
	DATAChunk data{};

	{
		char id[5];
		while (!stream.eof()) {
			memset(id, NULL, 5);
			stream.read(id, 4);
			stream.seekg(wavPoint);
			std::string str_id(id);
			if (str_id.compare("RIFF") == 0) {
				stream.read((char*)&riff, sizeof(RIFFChunk));
				wavPoint = (uint32_t)stream.tellg();
			}
			else if (str_id.compare("fmt ") == 0) {
				stream.read((char*)&fmt, sizeof(FMTChunk));
				wavPoint = (uint32_t)stream.tellg();
			}
			else if (str_id.compare("data") == 0) {
				stream.read((char*)&data, sizeof(DATAChunk));
				wavPoint = (uint32_t)stream.tellg();
				break;
			}
			else if (str_id.compare("junk") == 0) {
				JUNKChunk junk{};
				stream.read((char*)&junk, sizeof(JUNKChunk));
				wavPoint = (uint32_t)stream.tellg() + junk.chunk_size;
				stream.seekg(wavPoint);
			}
			else {
				CLog::Log(CLog::ELogLevel::ASLOG_ERROR, "Found unknown header\nFound header:" + str_id);
				stream.close();
				return false;
			}
		}
	}

	m_WaveInfo = AudioInfo(_directory, fmt.samplingRate, fmt.bitDepth, fmt.channels, (data.chunk_size / (fmt.bitDepth / BITPERBYTE)) / fmt.channels);
	m_HoldMode = _mode;

	std::string modeStr;
	switch (_mode) {
	case EHoldMode::WAVE_HOLDMODE_LOADALL: {
		std::unique_ptr<byte[]>pcmData(new byte[data.chunk_size]);

		stream.read((char*)pcmData.get(), data.chunk_size);
		stream.close();

		m_Wave.resize(m_WaveInfo.channel, m_WaveInfo.allFrames);

		PCM_Normalize(pcmData.get(), m_Wave, m_WaveInfo.bitdepth, m_WaveInfo.channel, m_WaveInfo.allFrames);

		modeStr = "LoadAll";
		break;
	}
	case EHoldMode::WAVE_HOLDMODE_STREAM:
		m_StreamPoint = wavPoint;
		m_WaveStream = std::move(stream);
		modeStr = "Stream";
		break;
	default:
		return false;
		break;
	}
	std::stringstream logstr;
	logstr << "Load wavfile" <<
		"\n\tHoleMode\t:\t" << modeStr <<
		"\n\tChannels\t:\t" << fmt.channels <<
		"\n\tFrequency\t:\t" << fmt.samplingRate <<
		"\n\tBitdepth\t:\t" << fmt.bitDepth <<
		"\n\tSize\t\t:\t" << data.chunk_size << "B" << "(" << (data.chunk_size / 1024) / 1024 << "MB" << std::flush;
	CLog::Log(CLog::ELogLevel::ASLOG_INFO, logstr.str());

	return true;
}

uint32_t CWavFile::GetStream(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames) {
	if (!m_WaveStream.is_open())return 0;

	uint16_t bdSize = m_WaveInfo.bitdepth / BITPERBYTE;
	uint32_t filePoint = (m_StreamPoint + (_srcPoint * m_WaveInfo.channel) * (bdSize));
	uint32_t streamSize = _frames * m_WaveInfo.channel * bdSize;
	//move file pointer
	if (m_WaveStream.tellg() != filePoint)
		m_WaveStream.seekg(filePoint, std::ios::beg);
	std::unique_ptr<byte[]> pcmData(new byte[streamSize]);
	m_WaveStream.read((char*)pcmData.get(), streamSize);

	PCM_Normalize(pcmData.get(), _dest, m_WaveInfo.bitdepth, m_WaveInfo.channel, _frames);
	return _frames;
}