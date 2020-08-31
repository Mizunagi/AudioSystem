#include "CAudioSystem.h"
#ifdef _WIN32
#include "CWasapi.h"
#endif

CAudioSystem::CAudioSystem() {
	CLog::Open(CLog::ELogLevel::ASLOG_ALL);
}

CAudioSystem::~CAudioSystem() {
}

void CAudioSystem::RenderThread() {
	CLineBuffer<float> renderBuffer;
	while (m_bRenderLoop) {
		FramesInfo frames;
		int32_t renderFrames = 0;

		if (!m_RenderDriver->WaitForProcess() || !m_RenderDriver->GetFrames(frames)) {
			StopInfo stop;
			m_RenderDriver->Stop(stop);
			m_bRenderLoop = false;
			break;
		}

		renderFrames = frames.paddingFrameSize != NOPADDING ? frames.paddingFrameSize : frames.maxFrameSize;
		if (renderBuffer.empty()) {
			renderBuffer.resize(m_RenderDriver->GetFormat().channel, frames.maxFrameSize);
		}
		renderBuffer.zeroclear();

		if (SPMasterTrack master = m_MasterTrack.lock()) {
			if (master->GetSourceSize() > 0) {
				master->GetBuffer(renderBuffer, renderFrames);
			}
		}

		m_RenderDriver->Process(renderBuffer, static_cast<int32_t>(renderFrames));
	}
	return;
}

bool CAudioSystem::LunchDevice(EDriverMode _mode, LunchInfo& _info) {
	std::unique_ptr<CDriverBase> instance;
#ifdef _WIN32
	instance = _info.lunchDevice.driverName == DRIVERNAME_WASAPI ? std::make_unique<CWasapi>() : nullptr;
#endif

	if (instance != nullptr && instance->LunchDevice(_info)) {
		switch (_info.lunchDevice.driverMode) {
		case EDriverMode::AS_DRIVERMODE_RENDER:
			m_RenderDriver = std::move(instance);
			return true;
		case EDriverMode::AS_DRIVERMODE_CAPTURE:
			m_CaptureDriver = std::move(instance);
			return true;
		}
	}
	return false;
}

bool CAudioSystem::SetupDevice(EDriverMode _mode, SetupInfo& _info) {
	switch (_mode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		return m_RenderDriver->SetupDevice(_info);
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		return m_CaptureDriver->SetupDevice(_info);
	}
	return false;
}

bool CAudioSystem::Start(EDriverMode _mode, StartInfo& _info) {
	switch (_mode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		if (m_RenderDriver->Start(_info)) {
			m_bRenderLoop = true;
			m_RenderThread = std::thread(&CAudioSystem::RenderThread, this);
			return true;
		}
		break;
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		if (m_CaptureDriver->Start(_info)) {
			m_bCaptureLoop = true;
			m_CaptureThread = std::thread(&CAudioSystem::CaptureThread, this);
		}
		break;
	}
	return false;
}

bool CAudioSystem::Stop(EDriverMode _mode, StopInfo& _info) {
	switch (_mode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		if (m_RenderDriver && m_RenderThread.joinable()) {
			m_bRenderLoop = false;
			m_RenderThread.join();
			return m_RenderDriver->Stop(_info);
		}
		return m_RenderDriver->Stop(_info);
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		if (m_CaptureDriver && m_CaptureThread.joinable()) {
			m_bCaptureLoop = false;
			m_CaptureThread.join();
			return m_CaptureDriver->Stop(_info);
		}
	}
	return false;
}

void CAudioSystem::Release(EDriverMode _mode) {
	switch (_mode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		m_RenderDriver.reset(nullptr);
		break;
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		m_CaptureDriver.reset(nullptr);
		break;
	}
}

bool CAudioSystem::CreateMasterTrack(SPMasterTrack& _master) {
	auto driverstate = m_RenderDriver->GetState();
	if (m_RenderDriver && (driverstate == EDriverState::AS_DRIVERSTATE_STOPPING || driverstate == EDriverState::AS_DRIVERSTATE_RENDERING)) {
		AudioFormat createformat = m_RenderDriver->GetFormat();
		FramesInfo frames;
		if (!m_RenderDriver->GetFrames(frames))return false;

		auto instance = CMasterTrack::CreateInstance(createformat, frames.maxFrameSize);
		m_MasterTrack = _master = instance;
		return true;
	}
	return false;
}

/// キャプチャソースの生成の場合ループ再生させるかwave保存させるかで引数に違いで発生するからその差を埋めないといけない
/// 提案：ソーストラック生成の段階ではwaveデータのバインドは行わずバッファの生成のみ行う
/// その後waveデータバインドはユーザーに行わせるー＞レンダーとキャプチャのソースの生成が１関数で行える

bool CAudioSystem::CreateSourceTrack(EDriverMode _mode, uint32_t _bufferTime, EEffectTiming _effectTiming, SPSourceTrack& _source) {
	auto rmd_createRender = [&] {
		if (!m_RenderDriver)return false;
		auto state = m_RenderDriver->GetState();
		if (state != EDriverState::AS_DRIVERSTATE_NONE && state != EDriverState::AS_DRIVERSTATE_CAPTURING) {
			if (auto master = m_MasterTrack.lock()) {
				AudioFormat deviceFormat = m_RenderDriver->GetFormat();
				FramesInfo deviceFrames;
				if (!m_RenderDriver->GetFrames(deviceFrames))return false;

				uint32_t orderFrames = static_cast<uint32_t>(std::lroundf(deviceFormat.samplingrate * (_bufferTime / 1000.0f)));
				uint32_t createFrames = 0;
				if (deviceFrames.maxFrameSize > orderFrames)
					createFrames = deviceFrames.maxFrameSize;
				else
					createFrames = orderFrames;

				_source = CSourceTrack::CreateInstance(deviceFormat, createFrames, _effectTiming);
				master->ConnectTrack(_source);
				return true;
			}
		}
		return false;
	};

	switch (_mode)
	{
	case EDriverMode::AS_DRIVERMODE_NONE:
		return false;
	case EDriverMode::AS_DRIVERMODE_RENDER:
		return rmd_createRender();
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		return false;
	default:
		return false;
	}
}

bool CAudioSystem::CreateEffector(const SPTrackBase& _track, WPEffectManager& _manager) {
	if (_track && _manager.expired()) {
		SPEffectManager insntace = CEffectManager::CreateInstance(_track->m_Format);
		_track->m_Effector = std::move(insntace);
		_manager = _track->m_Effector;
		return true;
	}
	return false;
}

DeviceInfo CAudioSystem::GetLunchedDeviceInfo(EDriverMode _mode) {
	return _mode == EDriverMode::AS_DRIVERMODE_RENDER ? m_RenderDriver->GetLunchedDeviceInfo()
		: _mode == EDriverMode::AS_DRIVERMODE_CAPTURE ? m_CaptureDriver->GetLunchedDeviceInfo()
		: DeviceInfo("", "", EDriverMode::AS_DRIVERMODE_NONE);
}