#pragma once
#include "CTrackBase.h"
#include "CWaveBase.h"

class CSourceTrack;
using SPSourceTrack = std::shared_ptr<CSourceTrack>;
using WPSourceTrack = std::weak_ptr<CSourceTrack>;

enum class EPlayState {
	AS_PLAYSTATE_NONE = 0,	//状態なし(NoBindWavedata)
	AS_PLAYSTATE_PLAY,		//再生中
	AS_PLAYSTATE_PAUSE,		//一時停止中
	AS_PLAYSTATE_STOPNEXT,	//次バッファで終了
	AS_PLAYSTATE_STOPREADY,	//現在バッファで終了
	AS_PLAYSTATE_STOP 		//停止中
};

enum class EEffectTiming {
	AS_EFFECTTIMING_IO,
	AS_EFFECTTIMING_SENDBUFFER
};

class CSourceTrack : public CTrackBase {
public:
	friend class CAudioSystem;
	template<typename T>
	friend class CStdPtrHelper;

	void Bind(const std::shared_ptr<CWaveBase> _wave);
	void Play() { Play(0); }
	void Play(int32_t _loop);
	void Pause();
	void Stop();
	EPlayState GetState() { return m_PlayState; }
	bool IsBinding() { return !m_Wave.expired() ? true : false; }
private:
	enum class ETrackNum {
		NONE = -1,
		PRIMARY,
		SECONDRY,
		DOUBLE_MAX
	};

	struct Track {
		CLineBuffer<float> trackBuffer;
		std::mutex trackMutex;
	};

	struct IORequest {
		CSourceTrack& source;
		Track& track;
		IORequest(CSourceTrack& _source, Track& _track) :source(_source), track(_track) {
		}
	};

	CSourceTrack(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming);
	~CSourceTrack();
	static SPSourceTrack CreateInstance(AudioFormat& _format, uint32_t _createFrames, EEffectTiming _effectTiming) { return CStdPtrHelper<CSourceTrack>::make_shared(_format, _createFrames, _effectTiming); }

	static std::thread m_sIOThread;
	static std::queue<IORequest> m_sIOReqQue;
	static std::condition_variable m_sIOVariable;
	static uint32_t m_sSourceInstances;
	static void SourceIOThread();

	EEffectTiming m_EffectTiming = EEffectTiming::AS_EFFECTTIMING_SENDBUFFER;
	EPlayState m_PlayState = EPlayState::AS_PLAYSTATE_NONE, m_TempState = EPlayState::AS_PLAYSTATE_NONE;
	std::array<Track, static_cast<size_t>(ETrackNum::DOUBLE_MAX)> m_Track;
	uint32_t m_UseTrack = 0;
	int32_t m_Loop = 0;
	std::weak_ptr<CWaveBase> m_Wave;

	uint32_t m_LoadCursor = 0;
	uint32_t m_SendCuesor = 0;

	size_t GetBuffer(CLineBuffer<float>& _buffer, uint32_t _frames)override;
	void Load(uint32_t _loadFrames, CLineBuffer<float>& _buffer);

	size_t ConnectTrack(const std::shared_ptr<CTrackBase> child) override { return 0; }
};
