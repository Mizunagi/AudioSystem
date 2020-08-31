#pragma once
#include "AudioDefine.h"
#include "CDriverBase.h"
#include "CMasterTrack.h"
#include "CSourceTrack.h"

class CAudioSystem final :public CSingleton<CAudioSystem> {
public:
	template<typename T>
	friend class CSingleton;

	template <class DRIVER>
	uint32_t GetDeviceList(EDriverMode _mode, DeviceList& _destList);
	bool LunchDevice(EDriverMode _mode, LunchInfo& _info);
	bool SetupDevice(EDriverMode _mode, SetupInfo& _info);
	bool Start(EDriverMode _mode, StartInfo& _info);
	bool Stop(EDriverMode _mode, StopInfo& _info);
	void Release(EDriverMode _mode);

	bool CreateMasterTrack(SPMasterTrack& _master);
	bool CreateSourceTrack(EDriverMode _mode, uint32_t _bufferTime, EEffectTiming _effectTiming, SPSourceTrack& _source);
	bool CreateEffector(const SPTrackBase&, WPEffectManager& _manager);
	DeviceInfo GetLunchedDeviceInfo(EDriverMode _mode);

private:
	CAudioSystem();
	~CAudioSystem();

	void RenderThread();
	void CaptureThread() {};

	std::thread m_RenderThread, m_CaptureThread;
	bool m_bRenderLoop = false, m_bCaptureLoop = false;

	WPMasterTrack m_MasterTrack;

	std::unique_ptr<CDriverBase> m_RenderDriver = nullptr, m_CaptureDriver = nullptr;
};

template<class DRIVER>
uint32_t CAudioSystem::GetDeviceList(EDriverMode _mode, DeviceList& _destList) {
	static_assert(std::is_base_of<CDriverBase, DRIVER>::value, "The specified type is not a derived type of CDriverBase");
	std::unique_ptr<CDriverBase> base(new DRIVER);
	return base->GetDeviceList(_mode, _destList);
}
