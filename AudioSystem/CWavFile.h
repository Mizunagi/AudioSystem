#pragma once
#include "CWaveBase.h"

struct RIFFChunk {
	char	chunk_ID[4];			//!RIFF�`�����N����['R''I''F''F']
	long	chunk_size = 0;			//!ID��size�������t�@�C���T�C�Y
	char	file_type[4];			//!�����f�[�^�̏ꍇ['W''A''V''E']
};

struct FMTChunk {
	char	chunk_ID[4];			//!FMT�`�����N����['f''m''t'' ']
	long	chunk_size = 0;			//!ID��size�������`�����N�f�[�^�T�C�Y
	short	wave_format_type = 0;	//!�g�`�f�[�^�̃t�H�[�}�b�g	PCM:0x0001
	short	channels = 0;			//!�g�p�`�����l����	Mono:1	Ste:2
	long	samplingRate = 0;		//!�T���v�����O���g��
	long	bytes_per_sec = 0;		//!1sec������̉��f�[�^(block_size*Sampling_Frequency)
	short	block_size = 0;			//!1�T���v��������̉��f�[�^(quantization*channel/8)
	short	bitDepth = 0;			//!�ʎq�����x
};

struct DATAChunk {
	char	chunk_ID[4];			//!DATA�`�����N����['d''a''t''a']
	long	chunk_size = 0;			//!ID��size�������f�[�^�`�����N�T�C�Y
};

//Junk�`�����N �W�����N�f�[�^
struct JUNKChunk {
	char	chunk_ID[4];
	long	chunk_size = 0;
};

class CWavFile : public CWaveBase {
public:
	~CWavFile() { if (m_WaveStream.is_open()) { m_WaveStream.close(); } }
	template<typename T>
	friend class CStdPtrHelper;

	static bool CreateInstance(std::string _directory, EHoldMode _mode, std::shared_ptr<CWavFile>& _wav) {
		bool result;
		if (_wav) {
			_wav.reset();
		}
		_wav = CStdPtrHelper<CWavFile>::make_shared(_directory, _mode, result);
		return result;
	}
private:
	CWavFile() {}
	CWavFile(std::string _directory, EHoldMode _mode, bool& _result) { _result = Load(_directory, _mode); };
	bool Load(std::string _directory, EHoldMode _mode);
	uint32_t GetStream(CLineBuffer<float>& _dest, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _frames);
};

using SPWavFile = std::shared_ptr<CWavFile>;