#ifdef _WIN32
#ifndef _CWASAPI_
#define _CWASAPI
#include "CDriverBase.h"
#include "CStdPtrHelper.h"

#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#pragma comment(lib,"Avrt.lib")
#pragma comment(lib,"winmm.lib")

#define DRIVERNAME_WASAPI "Wasapi"

struct WasapiLunchInfo :public LunchInfo {
	WasapiLunchInfo(DeviceInfo _device, AudioFormat _format, AUDCLNT_SHAREMODE _mode, AudioFormat* _paltFormat) :LunchInfo(_device, _format), shareMode(_mode), pAltFormat(_paltFormat) {}
	//WasapiMode(共有モード:AUDCLNT_SHAREMODE_SHARED 排他モード:AUDCLNT_SHAREMODE_EXCLUSIVE)
	AUDCLNT_SHAREMODE shareMode = AUDCLNT_SHAREMODE_SHARED;
	//AltanateFormat(Output)
	AudioFormat* pAltFormat = nullptr;
};

struct WasapiRenderSetupInfo :public SetupInfo {
	WasapiRenderSetupInfo(uint32_t _periodTime, DWORD _streamFlag) :periodTime(_periodTime), streamFlags(_streamFlag) {}
	//Render period(0=use device default period(
	uint32_t periodTime = 0;
	//Stream flags(AUDCLNT_STREAMFLAGS_~)
	DWORD streamFlags = 0;
};

struct WasapiCaptureSetupInfo :public SetupInfo {
};

struct WasapiStartInfo :public StartInfo {
	WasapiStartInfo(DWORD _timeOutTime) :timeoutTime(_timeOutTime) {}
	//event timeout time(millisecond)(sharemode->exclusive&streamflags->AUDCLNT_STREAMFLAGS_EVENTCALLBACK)
	DWORD timeoutTime = 0;
};

struct WasapiStopInfo :public StopInfo {
};

class CWasapi final : public CDriverBase {
public:
	CWasapi();
	~CWasapi();
	friend class CAudioSystem;

	static std::string GetDriverType() { return "Wasapi"; }
	uint32_t GetDeviceList(EDriverMode _mode, DeviceList& _destList) override;
	bool LunchDevice(LunchInfo& _info) override;
	bool SetupDevice(SetupInfo& _info) override;
	bool Start(StartInfo& _info) override;
	bool Stop(StopInfo& _info) override;

	bool WaitForProcess() override;
	bool GetFrames(FramesInfo& _info) override;
	int32_t Process(CLineBuffer<float>& outputBuffer, uint32_t renderFrames) override;
	DeviceInfo GetLunchedDeviceInfo() override;
private:
	static	CLSID	m_sCLSID_MMDeviceEnumerator;
	static	IID		m_sIID_IMMDeviceEnumerator;
	static	IID		m_sIID_IAudioClient;
	static	IID		m_sIID_IAudioClock;
	static	IID		m_sIID_IAudioRenderClient;
	static	IID		m_sIID_IAudioCaptureClient;
	static std::map<std::string, IMMDevice*> m_sMapDevice;
	static std::map<HRESULT, std::string> m_sErrorDetails;

	EDriverMode m_DriverMode = EDriverMode::AS_DRIVERMODE_NONE;
	IMMDevice* m_pDevice = nullptr;
	IAudioClient* m_pClient = nullptr;
	AUDCLNT_SHAREMODE m_ShareMode = static_cast<AUDCLNT_SHAREMODE>(0);
	REFERENCE_TIME m_WaitTime = 0, m_PeriodTime = 0, m_MinPeriod = 0;
	HANDLE m_hEventHandle = nullptr;
	DWORD m_TimeOutTime = 0;
	uint32_t m_FrameSize = 0, m_OneFrameSize = 0;

	IAudioRenderClient* m_pRenderClient = nullptr;
	IAudioCaptureClient* m_pCaptureClient = nullptr;

	bool RenderSetupDevice(WasapiRenderSetupInfo& _info);
	bool CaptureSetupDevice(WasapiCaptureSetupInfo& _info);
	int32_t RenderProcess(CLineBuffer<float>& outputBuffer, uint32_t& renderFrames);
	int32_t CaptureProcess(CLineBuffer<float>& outputBuffer, uint32_t& renderFrames);
	bool CheckError(HRESULT& _result, std::string _method, std::string _function, std::string _successStr, bool _successView);
	HRESULT CheckFormat(IAudioClient* _pClient, AudioFormat& _lunchFormat, AUDCLNT_SHAREMODE& _shareMode, AudioFormat& _destFormat);
	IMMDevice* GetDefaultDevice(EDriverMode& _mode);
};

WAVEFORMATEXTENSIBLE	SetWFXfromAudioFormat(AudioFormat& _info);
AudioFormat	SetAudioFormatfromWFX(WAVEFORMATEXTENSIBLE& _WFX);
#endif
#endif