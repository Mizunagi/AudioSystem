#include "CSourceTrack.h"

std::thread CSourceTrack::m_sIOThread;
std::queue<CSourceTrack::IORequest> CSourceTrack::m_sIOReqQue;
std::condition_variable CSourceTrack::m_sIOVariable;
uint32_t CSourceTrack::m_sSourceInstances = 0;

CSourceTrack::CSourceTrack(AudioFormat _format, uint32_t _createFrames, EEffectTiming _effectTiming) {
	m_Format = _format;
	m_Frames = _createFrames;
	if (m_sSourceInstances++ <= 0)
		//lunch IO thread
		m_sIOThread = std::thread(CSourceTrack::SourceIOThread);

	//create buffer
	for (auto& track : m_Track) {
		track.trackBuffer.resize(m_Format.channel, _createFrames);
		track.trackBuffer.zeroclear();
	}

	m_EffectTiming = _effectTiming;
	m_PlayState = EPlayState::AS_PLAYSTATE_STOP;

	std::stringstream logstr;
	logstr << "Create SourceTrack\n"
		<< "Create Format\n"
		<< "\tChannels\t:\t" << m_Format.channel << "\n"
		<< "\tSamplingRate\t:\t" << m_Format.samplingrate << "\n"
		<< "\tBitDepth\t:\t" << m_Format.bitdepth << "\n"
		<< "Created Source Buffer Frame\t:\t" << _createFrames;
	CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", logstr.str());
}

CSourceTrack::~CSourceTrack() {
	Stop();
	if (--m_sSourceInstances <= 0 && m_sIOThread.joinable()) {
		//end IO thread
		m_sIOVariable.notify_all();
		m_sIOThread.join();
	}
}

void CSourceTrack::SourceIOThread() {
	std::mutex ioMutex;
	std::unique_lock<std::mutex> uniqMutex(ioMutex);

	while (m_sSourceInstances > 0) {
		while (!m_sIOReqQue.empty()) {
			auto& request = m_sIOReqQue.front();
			request.track.trackMutex.lock();
			request.source.Load(request.source.m_Frames, request.track.trackBuffer);
			if (request.source.m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_IO && request.source.m_Effector && request.source.m_Effector->GetEffectSize() != 0)
				request.source.m_Effector->Execute(request.track.trackBuffer, request.track.trackBuffer.sizeX());
			request.track.trackMutex.unlock();
			m_sIOReqQue.pop();
		}
		m_sIOVariable.wait(uniqMutex, [&] {return m_sSourceInstances <= 0 || !m_sIOReqQue.empty(); });
	}
}

void CSourceTrack::Bind(const std::shared_ptr<CWaveBase> _wave) {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE || m_PlayState == EPlayState::AS_PLAYSTATE_LASTFRAME || m_PlayState == EPlayState::AS_PLAYSTATE_LASTBUFFER) {
		Stop();
	}
	if (_wave && m_PlayState == EPlayState::AS_PLAYSTATE_NONE || m_PlayState == EPlayState::AS_PLAYSTATE_STOP) {
		m_Wave = _wave;
		m_SendCuesor = 0;
		std::stringstream logstr;
		logstr << "Bind to Wavedata\n"
			<< "\tBind File Directory\t:\t" << _wave->GetInfo().name;
		CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", logstr.str());
	}
}

void CSourceTrack::Play(int32_t _loop) {
	m_Loop = _loop >= 0 ? _loop : _loop == LOOP_INFINITY ? LOOP_INFINITY : 0;
	if (m_PlayState == EPlayState::AS_PLAYSTATE_STOP) {
		m_PlayState = EPlayState::AS_PLAYSTATE_PLAY;
		CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", "");
	}
	else if (m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE) {
		m_PlayState = m_TempState;
		m_TempState = EPlayState::AS_PLAYSTATE_NONE;
		CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", "");
	}
}

void CSourceTrack::Pause() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY || m_PlayState == EPlayState::AS_PLAYSTATE_LASTFRAME || m_PlayState == EPlayState::AS_PLAYSTATE_LASTBUFFER) {
		m_TempState = m_PlayState;
		m_PlayState = EPlayState::AS_PLAYSTATE_PAUSE;
		CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", "");
	}
}

void CSourceTrack::Stop() {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_PLAY || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE || m_PlayState == EPlayState::AS_PLAYSTATE_LASTFRAME || m_PlayState == EPlayState::AS_PLAYSTATE_LASTBUFFER) {
		m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
		m_TempState = EPlayState::AS_PLAYSTATE_NONE;
		m_LoadCursor = m_SendCuesor = 0;
		if (m_Effector) {
			m_Effector->Flush();
		}
		for (auto& track : m_Track)
			track.trackBuffer.zeroclear();
		CLog::Log(CLog::ASLOG_INFO, std::string(typeid(CSourceTrack).name()), __func__, "", "");
	}
}

size_t CSourceTrack::GetBuffer(CLineBuffer<float>& _buffer, uint32_t _frames) {
	if (m_PlayState == EPlayState::AS_PLAYSTATE_NONE || m_PlayState == EPlayState::AS_PLAYSTATE_STOP || m_PlayState == EPlayState::AS_PLAYSTATE_PAUSE)
		return 0;

	auto rmd_avx_mul = [=](CLineBuffer<float>& _buf, AudioFormat _format, float _mul) {
		const uint32_t avxSize = sizeof(__m256) / sizeof(float);
		const __m256 mul = _mm256_set_ps(_mul, _mul, _mul, _mul, _mul, _mul, _mul, _mul);
		for (uint32_t c = 0; c < _format.channel; ++c) {
			for (uint32_t f = 0; f < _buf.sizeRealX(); f += avxSize) {
				__m256 a = _mm256_load_ps(&_buf[c][f]);
				__m256 res = _mm256_mul_ps(a, mul);
				_mm256_store_ps(&_buf[c][f], res);
			}
		}
	};

	auto rmd_swap = [=](Track& _pri, Track& _scnd) {
		_scnd.trackMutex.lock();
		_pri.trackBuffer.swap(_scnd.trackBuffer);
		_scnd.trackMutex.unlock();
	};

	auto rmd_copy = [=](CLineBuffer<float>& _dest, CLineBuffer<float>& _src, uint32_t _destPoint, uint32_t _srcPoint, uint32_t _copyFrames) {
		for (uint32_t c = 0; c < m_Format.channel; ++c) {
			std::memcpy(&_dest[c][_destPoint], &_src[c][_srcPoint], sizeof(float) * _copyFrames);
		}
	};

	auto& primaty = m_Track[static_cast<size_t>(ETrackNum::PRIMARY)];
	auto& secondry = m_Track[static_cast<size_t>(ETrackNum::SECONDRY)];

	//Critical Section
	{
		std::lock_guard lock(primaty.trackMutex);
		if (m_SendCuesor + _frames < m_Frames) {
			//send
			rmd_copy(_buffer, primaty.trackBuffer, 0, m_SendCuesor, _frames);
			//move cursor
			m_SendCuesor += _frames;
			rmd_avx_mul(_buffer, m_Format, m_Volume);
		}
		else {
			uint32_t endFrames = m_Frames - m_SendCuesor;
			uint32_t beginFrames = _frames - endFrames;
			rmd_copy(_buffer, primaty.trackBuffer, 0, m_SendCuesor, endFrames);

			if (m_PlayState != EPlayState::AS_PLAYSTATE_LASTBUFFER) {
				rmd_swap(primaty, secondry);
				rmd_copy(_buffer, primaty.trackBuffer, endFrames, 0, beginFrames);
				m_SendCuesor = beginFrames;
				if (m_PlayState != EPlayState::AS_PLAYSTATE_LASTFRAME) {
					m_sIOReqQue.push(IORequest(*this, secondry));
					m_sIOVariable.notify_one();
				}
				else {
					m_PlayState = EPlayState::AS_PLAYSTATE_LASTBUFFER;
				}
				rmd_avx_mul(_buffer, m_Format, m_Volume);
			}
			else {
				//end play
				m_PlayState = EPlayState::AS_PLAYSTATE_STOP;
				m_LoadCursor = m_SendCuesor = m_Loop = 0;
				for (auto& track : m_Track)
					track.trackBuffer.zeroclear();
				if (m_StopCallback)m_StopCallback();
			}
		}
	}

	if (m_EffectTiming == EEffectTiming::AS_EFFECTTIMING_SENDBUFFER && m_Effector && m_Effector->GetEffectSize() != 0)
		m_Effector->Execute(_buffer, _frames);

	return _frames;
}

void CSourceTrack::Load(uint32_t _loadFrames, CLineBuffer<float>& _buffer) {
	if (auto wav = m_Wave.lock()) {
		auto waveInfo = wav->GetInfo();
		_buffer.zeroclear();
		if ((m_LoadCursor + _loadFrames) < waveInfo.allFrames) {
			wav->GetBuffer(_buffer, 0, m_LoadCursor, _loadFrames);
			m_LoadCursor += _loadFrames;
		}
		else {
			uint32_t endFrame = waveInfo.allFrames - m_LoadCursor;
			uint32_t beginFrame = _loadFrames - endFrame;
			wav->GetBuffer(_buffer, 0, m_LoadCursor, endFrame);
			if (m_Loop == LOOP_INFINITY || m_Loop > 0) {
				wav->GetBuffer(_buffer, endFrame, 0, beginFrame);
				m_LoadCursor = beginFrame;
				m_PlayState = EPlayState::AS_PLAYSTATE_PLAY;
				m_Loop--;
			}
			else if (m_Loop == 0) {
				m_PlayState = EPlayState::AS_PLAYSTATE_LASTFRAME;
				m_LoadCursor = 0;
			}
		}
	}
}