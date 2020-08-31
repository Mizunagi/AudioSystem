#pragma once
#include "CWaveBase.h"

struct RIFFChunk {
	char	chunk_ID[4];			//!RIFFチャンク識別['R''I''F''F']
	long	chunk_size = 0;			//!IDとsizeを除くファイルサイズ
	char	file_type[4];			//!音声データの場合['W''A''V''E']
};

struct FMTChunk {
	char	chunk_ID[4];			//!FMTチャンク識別['f''m''t'' ']
	long	chunk_size = 0;			//!IDとsizeを除くチャンクデータサイズ
	short	wave_format_type = 0;	//!波形データのフォーマット	PCM:0x0001
	short	channels = 0;			//!使用チャンネル数	Mono:1	Ste:2
	long	samplingRate = 0;		//!サンプリング周波数
	long	bytes_per_sec = 0;		//!1sec当たりの音データ(block_size*Sampling_Frequency)
	short	block_size = 0;			//!1サンプル当たりの音データ(quantization*channel/8)
	short	bitDepth = 0;			//!量子化精度
};

struct DATAChunk {
	char	chunk_ID[4];			//!DATAチャンク識別['d''a''t''a']
	long	chunk_size = 0;			//!IDとsizeを除くデータチャンクサイズ
};

//Junkチャンク ジャンクデータ
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