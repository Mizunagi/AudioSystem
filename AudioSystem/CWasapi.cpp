#include "CWasapi.h"

CLSID	CWasapi::m_sCLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
IID		CWasapi::m_sIID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
IID		CWasapi::m_sIID_IAudioClient = __uuidof(IAudioClient);
IID		CWasapi::m_sIID_IAudioClock = __uuidof(IAudioClock);
IID		CWasapi::m_sIID_IAudioRenderClient = __uuidof(IAudioRenderClient);
IID		CWasapi::m_sIID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
std::map<std::string, IMMDevice*>	CWasapi::m_sMapDevice;
std::map<HRESULT, std::string>		CWasapi::m_sErrorDetails{
	{E_NOTIMPL,"指定された機能が実装されていません。"},
	{E_NOINTERFACE,"要求された COM インターフェイスは使用できません。"},
	{E_POINTER,"無効なポインターが使用されました。"},
	{E_ABORT,"操作が中止されました。"},
	{E_FAIL,"一般的な障害が発生しました。"},
	{REGDB_E_CLASSNOTREG,"必要な COM オブジェクトが登録されていません。"},
	{CO_E_NOTINITIALIZED,"CoInitialize が呼び出されていません。"},
	{ERROR_INVALID_FUNCTION,"無効な関数が呼び出されました。"},
	{ERROR_FILE_NOT_FOUND,"指定されたファイルが見つかりませんでした。"},
	{ERROR_PATH_NOT_FOUND,"指定されたパスが見つかりませんでした。"},
	{E_ACCESSDENIED,"リソースへのアクセスが拒否されました。"},
	{E_HANDLE,"無効なオブジェクト ハンドルが使用されました。"},
	{E_OUTOFMEMORY,"割り当て可能なメモリーがありません。"},
	{E_INVALIDARG,"無効な引数が検出されました。"},
	{ERROR_GEN_FAILURE,"システムに接続されたデバイスが機能していません。"},
	{ERROR_MOD_NOT_FOUND,"必要な DLL または EXE が見つかりませんでした。"},
	{ERROR_MAX_THRDS_REACHED,"システムの最大スレッド数に達しました。"},
	{ERROR_ALREADY_EXISTS,"既に存在しているため、ファイルを作成できませんでした。"},
	{ERROR_NO_SYSTEM_RESOURCES,"必要なシステム リソースが使い果たされています。"},
	{RPC_S_UNKNOWN_IF,"そのインターフェイスは認識されません。"},
	{RPC_S_CALL_FAILED,"リモート プロシージャ コールに失敗しました。"},
	{ERROR_PARTIAL_COPY,"ReadProcessMemory 要求または WriteProcessMemory 要求の一部だけを完了しました。"},
	{AUDCLNT_E_NOT_INITIALIZED,"Not Initualized"},
	{AUDCLNT_E_ALREADY_INITIALIZED,"Already Initialized"},
	{AUDCLNT_E_WRONG_ENDPOINT_TYPE,"Wrong Endpoint Type"},
	{AUDCLNT_E_DEVICE_INVALIDATED,"Device Invalidated"},
	{AUDCLNT_E_NOT_STOPPED,"Not Stopped"},
	{AUDCLNT_E_BUFFER_TOO_LARGE,"Buffer Too Large"},
	{AUDCLNT_E_OUT_OF_ORDER,"Out of Order"},
	{AUDCLNT_E_UNSUPPORTED_FORMAT,"Unsupported Format"},
	{AUDCLNT_E_INVALID_SIZE,"Invalid Size"},
	{AUDCLNT_E_DEVICE_IN_USE,"Device in Use"},
	{AUDCLNT_E_BUFFER_OPERATION_PENDING,"Buffer Operation Pending"},
	{AUDCLNT_E_THREAD_NOT_REGISTERED,"Thread Not Registered"},
	{AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED,"Not Allowed Exclusive Mode"},
	{AUDCLNT_E_ENDPOINT_CREATE_FAILED,"Endpoint Create Failed"},
	{AUDCLNT_E_SERVICE_NOT_RUNNING,"Service Not Running"},
	{AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED,"Eventhandle Not Expected"},
	{AUDCLNT_E_EXCLUSIVE_MODE_ONLY,"Exclusive Mode Only"},
	{AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL,"Bufduration Period Not Equal"},
	{AUDCLNT_E_EVENTHANDLE_NOT_SET,"Eventhandle Not Set"},
	{AUDCLNT_E_INCORRECT_BUFFER_SIZE,"Incorrect Buffer Size"},
	{AUDCLNT_E_BUFFER_SIZE_ERROR,"Buffer Size Error"},
	{AUDCLNT_E_CPUUSAGE_EXCEEDED,"Cpuusage Exceeded"},
	{AUDCLNT_E_BUFFER_ERROR,"Buffer Error"},
	{AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED,"Buffer Size Not Aligned"},
	{AUDCLNT_E_INVALID_DEVICE_PERIOD,"Invalid Device Period"},
	{AUDCLNT_E_INVALID_STREAM_FLAG,"Invalid Stream Flag"},
	{AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE,"Endpoint Offload Not Capable"},
	{AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES,"Out of Offload Resources"},
	{AUDCLNT_E_OFFLOAD_MODE_ONLY,"Offload Mode Only"},
	{AUDCLNT_E_NONOFFLOAD_MODE_ONLY,"Non Offload Mode Only"},
	{AUDCLNT_E_RESOURCES_INVALIDATED,"Resources Incalodated"},
	{AUDCLNT_E_RAW_MODE_UNSUPPORTED,"Raw Mode Unsupported"}
};

CWasapi::CWasapi() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "CoInitializeEx", "Initialized COM");
}

CWasapi::~CWasapi() {
	if (m_hEventHandle) {
		CloseHandle(m_hEventHandle);
		m_hEventHandle = nullptr;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "CloseHandle", "Closed Event Handle");
	}
	if (m_pRenderClient) {
		m_pRenderClient->Release();
		m_pRenderClient = nullptr;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "IAudioRenderClient->Release", "Released Render Client");
	}
	if (m_pCaptureClient) {
		m_pCaptureClient->Release();
		m_pCaptureClient = nullptr;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "IAudioCaptureClient->Release", "Released Capture Client");
	}
	if (m_pClient) {
		m_pClient->Release();
		m_pClient = nullptr;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "IAudioClient->Release", "Released Audio Client");
	}
	if (m_pDevice) {
		m_pDevice->Release();
		m_pDevice = nullptr;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "IMMDevice->Release", "Released IMMDevice");
	}
	CoUninitialize();
	CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "CoUninitialize", "Uninitialized COM");
}

uint32_t CWasapi::GetDeviceList(EDriverMode _mode, DeviceList& _destList) {
	HRESULT result;
	IMMDeviceEnumerator* pDeviceEnum = nullptr;
	IMMDeviceCollection* pDeviceCollect = nullptr;
	uint32_t deviceCount = 0;

	if (!m_sMapDevice.empty())m_sMapDevice.clear();

	//Create enumerator
	result = CoCreateInstance(m_sCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, m_sIID_IMMDeviceEnumerator, (void**)&pDeviceEnum);
	if (!CheckError(result, __func__, "CoCreateInstance", "Success Create Enumerator", true))return 0;

	//Enumerate Audio End Points
	EDataFlow dataflow = _mode == EDriverMode::AS_DRIVERMODE_RENDER ? EDataFlow::eRender : _mode == EDriverMode::AS_DRIVERMODE_CAPTURE ? EDataFlow::eCapture : (EDataFlow)-1;
	result = pDeviceEnum->EnumAudioEndpoints(dataflow, DEVICE_STATE_ACTIVE, &pDeviceCollect);
	if (!CheckError(result, __func__, "EnumAudioEndpoints", "Success Enumerate Devices", true))return 0;

	result = pDeviceCollect->GetCount(&deviceCount);
	if (!CheckError(result, __func__, "GetCount", "Success Get Device Count>Devices:" + std::to_string(deviceCount), true))return 0;

	for (uint32_t i = 0; i < deviceCount; ++i) {
		IMMDevice* pDevice = nullptr;
		IPropertyStore* pPropStore = nullptr;
		PROPVARIANT propVer{};

		//GetDevive
		pDeviceCollect->Item(i, &pDevice);
		//Get Device Property
		pDevice->OpenPropertyStore(STGM_READ, &pPropStore);
		PropVariantInit(&propVer);
		pPropStore->GetValue(PKEY_Device_DeviceDesc, &propVer);

		DeviceInfo info(Convert_Wstr_to_Str(std::wstring(propVer.bstrVal)), GetDriverType(), _mode);
		_destList.push_back(info);
		m_sMapDevice.insert(std::make_pair(info.deviceName, pDevice));
		PropVariantClear(&propVer);

		std::stringstream strstr;
		std::string _m = _mode == EDriverMode::AS_DRIVERMODE_RENDER ?
			"Render" : _mode == EDriverMode::AS_DRIVERMODE_CAPTURE ?
			"Capture" : "Unknown";
		strstr << "IDX\t\t:\t" << i <<
			"\nDevice Name\t:\t" << info.deviceName <<
			"\nDriver Name\t:\t" << info.driverName <<
			"\nDriver Mode\t:\t" << _m << std::flush;
		CLog::Log(CLog::ELogLevel::ASLOG_INFO, strstr.str());
	}

	pDeviceCollect->Release();
	pDeviceEnum->Release();

	return deviceCount;
}

bool CWasapi::LunchDevice(LunchInfo& _info) {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_NONE) return false;

	HRESULT result;

	WasapiLunchInfo& wa_info = static_cast<WasapiLunchInfo&>(_info);

	if (!_info.lunchDevice.deviceName.empty()) {
		//recreate Devicemap
		DeviceList l;
		if (!GetDeviceList(wa_info.lunchDevice.driverMode, l))return false;
		//Get Device From Devicemap
		m_pDevice = m_sMapDevice[_info.lunchDevice.deviceName];
	}
	else {
		m_pDevice = GetDefaultDevice(_info.lunchDevice.driverMode);
	}
	if (!m_pDevice)return false;
	if (m_pClient) {
		m_pClient->Release();
		m_pClient = nullptr;
	}

	result = m_pDevice->Activate(m_sIID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&m_pClient);
	if (!CheckError(result, __func__, "Activate", "Activate Device", true))return false;

	result = CheckFormat(m_pClient, wa_info.lunchFormat, wa_info.shareMode, m_Format);
	if (result != S_OK) {
		m_pClient->Release();
		m_pClient = nullptr;
		return false;
	}

	m_ShareMode = wa_info.shareMode;

	m_DriverMode = wa_info.lunchDevice.driverMode;

	m_DriverState = EDriverState::AS_DRIVERSTATE_LUNCHED;

	return true;
}

bool CWasapi::RenderSetupDevice(WasapiRenderSetupInfo& _info) {
	HRESULT result;
	if (_info.periodTime != 0) {
		m_WaitTime = static_cast<REFERENCE_TIME>(_info.periodTime);
		m_PeriodTime = m_WaitTime * 10000LL;
	}
	else {
		//Get Device Default Period
		result = m_pClient->GetDevicePeriod(&m_PeriodTime, &m_MinPeriod);
		if (!CheckError(result, __func__, "GetDevicePeriod", "Get Device Default Period", true))return false;
		m_WaitTime = m_PeriodTime / 10000LL;
	}

	result = m_pClient->Initialize(m_ShareMode, _info.streamFlags, m_PeriodTime, m_PeriodTime, (WAVEFORMATEX*)&SetWFXfromAudioFormat(m_Format), nullptr);
	if (!CheckError(result, __func__, "Initialize", "Initialize Render Client", true)) {
		//fix buffer align
		if (result == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
			uint32_t deviceFrame = 0;
			m_pClient->GetBufferSize(&deviceFrame);
			REFERENCE_TIME fixPeriod = static_cast<REFERENCE_TIME>(
				10000.0 *					// (REFERENCE_TIME(100ns) / ms) *
				1000 *						// (ms / s) *
				deviceFrame /				// frames /
				m_Format.samplingrate +		// (frames / s)
				0.5							// 四捨五入
				);
			//recreate client
			m_pClient->Release();
			m_pDevice->Activate(m_sIID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&m_pClient);
			result = m_pClient->Initialize(m_ShareMode, _info.streamFlags, fixPeriod, fixPeriod, (WAVEFORMATEX*)&SetWFXfromAudioFormat(m_Format), nullptr);
			if (!CheckError(result, __func__, "Initialize", "Initialize Render Client(fixed period)", true))
				return false;
		}
	}

	//Event Driven Check(Only Exclusivemode)
	if (m_ShareMode == AUDCLNT_SHAREMODE_EXCLUSIVE && _info.streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) {
		m_hEventHandle = CreateEvent(nullptr, false, false, nullptr);
		if (!m_hEventHandle) {
			CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), __func__, "CreateEvent", "Failed Create Event");
			return false;
		}
		result = m_pClient->SetEventHandle(m_hEventHandle);
		if (!CheckError(result, __func__, "SetEventHandle", "Set Event Handle", true)) return false;
	}

	byte* pBuf = nullptr;
	result = m_pClient->GetService(m_sIID_IAudioRenderClient, (void**)&m_pRenderClient);
	if (!CheckError(result, __func__, "GetService", "Succeeded get render service", true))
		return false;

	result = m_pClient->GetBufferSize(&m_FrameSize);
	if (!CheckError(result, __func__, "GetBufferSize", "Succeeded get render frame size \nFramesize\t:\t" + std::to_string(m_FrameSize), true))return false;
	m_OneFrameSize = m_Format.channel * (m_Format.bitdepth / BITPERBYTE);

	//初回バッファクリア
	m_pRenderClient->GetBuffer(m_FrameSize, &pBuf);
	memset(pBuf, NULL, static_cast<size_t>(sizeof(byte) * (m_FrameSize * m_OneFrameSize)));
	m_pRenderClient->ReleaseBuffer(m_FrameSize, 0);
	return true;
}

bool CWasapi::CaptureSetupDevice(WasapiCaptureSetupInfo& _info) {
	return false;
}

bool CWasapi::SetupDevice(SetupInfo& _info) {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_LUNCHED)return false;
	bool result = false;
	switch (m_DriverMode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		result = RenderSetupDevice((WasapiRenderSetupInfo&)_info);
		break;
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		result = CaptureSetupDevice((WasapiCaptureSetupInfo&)_info);
		break;
	}

	if (result)
		m_DriverState = EDriverState::AS_DRIVERSTATE_STOPPING;

	return result;
}

bool CWasapi::Start(StartInfo& _info) {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_STOPPING)
		return false;

	WasapiStartInfo& wa_info = static_cast<WasapiStartInfo&>(_info);

	HRESULT result = m_pClient->Start();
	if (!CheckError(result, __func__, "Start", "Start Process", true))return false;

	m_TimeOutTime = wa_info.timeoutTime;

	switch (m_DriverMode) {
	case EDriverMode::AS_DRIVERMODE_RENDER:
		m_DriverState = EDriverState::AS_DRIVERSTATE_RENDERING;
		break;
	case EDriverMode::AS_DRIVERMODE_CAPTURE:
		m_DriverState = EDriverState::AS_DRIVERSTATE_CAPTURING;
		return false;
		break;
	}

	return true;
}

bool CWasapi::Stop(StopInfo& _info) {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_RENDERING && m_DriverState != EDriverState::AS_DRIVERSTATE_CAPTURING)
		return false;

	HRESULT result = m_pClient->Stop();
	if (!CheckError(result, __func__, "Stop", "Stop Process", true))return false;

	m_DriverState = EDriverState::AS_DRIVERSTATE_STOPPING;

	return false;
}

bool CWasapi::WaitForProcess() {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_RENDERING && m_DriverState != EDriverState::AS_DRIVERSTATE_CAPTURING)
		return false;
	if (m_hEventHandle) {
		//event driven
		DWORD retval = WaitForSingleObject(m_hEventHandle, m_TimeOutTime);
		if (retval != WAIT_OBJECT_0) {
			switch (retval) {
			case WAIT_TIMEOUT:
				CLog::Log(CLog::ELogLevel::ASLOG_ERROR, std::string(typeid(CWasapi).name()), __func__, "WaitForSingleObject", "Timeout Event Object!");
				break;
			case WAIT_FAILED:
				CLog::Log(CLog::ELogLevel::ASLOG_ERROR, std::string(typeid(CWasapi).name()), __func__, "WaitForSingleObject", "Event Object Error!");
				LPVOID lpMsgBuf;
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
				LocalFree(lpMsgBuf);
				break;
			default:
				break;
			}
			return false;
		}
	}
	else {
		//timer
		std::this_thread::sleep_for(std::chrono::milliseconds(m_WaitTime));
	}

	return true;
}

bool CWasapi::GetFrames(FramesInfo& _info) {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_STOPPING && m_DriverState != EDriverState::AS_DRIVERSTATE_RENDERING && m_DriverState != EDriverState::AS_DRIVERSTATE_CAPTURING)
		return false;

	HRESULT result;
	uint32_t padding = 0;

	//get padding frames
	result = m_pClient->GetCurrentPadding(&padding);
	if (!CheckError(result, __func__, "GetCurrentPadding", "PaddingFrames\t:\t" + std::to_string(m_FrameSize - padding), false))return false;

	padding = m_FrameSize - padding;
	_info.maxFrameSize = m_FrameSize;
	_info.paddingFrameSize = m_ShareMode == AUDCLNT_SHAREMODE_SHARED ? static_cast<int32_t>(padding) : NOPADDING;

	return true;
}

int32_t CWasapi::Process(CLineBuffer<float>& outputBuffer, uint32_t renderFrames) {
	return m_DriverState == EDriverState::AS_DRIVERSTATE_RENDERING ? RenderProcess(outputBuffer, renderFrames) : m_DriverState == EDriverState::AS_DRIVERSTATE_CAPTURING ? CaptureProcess(outputBuffer, renderFrames) : 0;
}

DeviceInfo CWasapi::GetLunchedDeviceInfo() {
	if (m_DriverState != EDriverState::AS_DRIVERSTATE_NONE) {
		IPropertyStore* pPropStore = nullptr;
		PROPVARIANT propVer{};

		//Get Device Property
		m_pDevice->OpenPropertyStore(STGM_READ, &pPropStore);
		PropVariantInit(&propVer);
		pPropStore->GetValue(PKEY_Device_DeviceDesc, &propVer);

		return DeviceInfo(Convert_Wstr_to_Str(std::wstring(propVer.bstrVal)), GetDriverType(), m_DriverMode);
	}
	return DeviceInfo("", "", EDriverMode::AS_DRIVERMODE_NONE);
}

int32_t CWasapi::RenderProcess(CLineBuffer<float>& outputBuffer, uint32_t& renderFrames) {
	HRESULT result;
	byte* pBuffer = nullptr;

	result = m_pRenderClient->GetBuffer(renderFrames, &pBuffer);
	if (!CheckError(result, __func__, "GetBuffer", "", false)) {
		WasapiStopInfo stop;
		Stop(stop);
		return PROCESSERROR;
	}

	//Memory Clear
	memset(pBuffer, NULL, static_cast<size_t>(renderFrames * m_OneFrameSize));

	PCM_Denormalize(outputBuffer, pBuffer, m_Format.bitdepth, m_Format.channel, renderFrames);

	result = m_pRenderClient->ReleaseBuffer(renderFrames, 0);
	if (!CheckError(result, __func__, "ReleaseBuffer", "", false)) {
		WasapiStopInfo stop;
		Stop(stop);
		return PROCESSERROR;
	}

	return renderFrames;
}

int32_t CWasapi::CaptureProcess(CLineBuffer<float>& outputBuffer, uint32_t& captureFrames) {
	return PROCESSERROR;
}

bool CWasapi::CheckError(HRESULT& _result, std::string _method, std::string _function, std::string _successStr, bool _successView) {
	std::stringstream outputStream;
	if (_result == S_OK) {
		if (_successView)
			CLog::Log(CLog::ELogLevel::ASLOG_INFO, std::string(typeid(CWasapi).name()), _method, _function, _successStr);
		return true;
	}
	else {
		outputStream << "\nErrorCode\t:\t" << std::hex << _result << std::dec <<
			"\nDetail\t\t:\t" << m_sErrorDetails[_result] << std::flush;
		CLog::Log(CLog::ELogLevel::ASLOG_ERROR, std::string(typeid(CWasapi).name()), _method, _function, outputStream.str());
		return false;
	}
}

HRESULT CWasapi::CheckFormat(IAudioClient* _pClient, AudioFormat& _lunchFormat, AUDCLNT_SHAREMODE& _shareMode, AudioFormat& _destFormat) {
	WAVEFORMATEXTENSIBLE wfx = SetWFXfromAudioFormat(_lunchFormat), * pAltFormat;
	HRESULT result = _pClient->IsFormatSupported(_shareMode, (WAVEFORMATEX*)&wfx, (WAVEFORMATEX**)&pAltFormat);

	std::stringstream outputStr;
	auto rmd_lunchFormat = [=](std::stringstream& str) {
		str << "LunchFormat\n" <<
			"\tChannel\t\t\t:\t" << _lunchFormat.channel << "\n" <<
			"\tSampling Rate\t\t:\t" << _lunchFormat.samplingrate << "\n" <<
			"\tBit Depth\t\t:\t" << _lunchFormat.bitdepth << "\n" << std::flush;
	};

	CLog::ELogLevel level = CLog::ELogLevel::ASLOG_INFO;
	if (result == S_OK) {
		rmd_lunchFormat(outputStr);
		_destFormat = SetAudioFormatfromWFX(wfx);
	}
	else if (result == S_FALSE) {
		AudioFormat altFormat = SetAudioFormatfromWFX(*pAltFormat);
		outputStr << "Output alternate format\n" <<
			"\tChannel\t\t\t:\t" << altFormat.channel << "\n" <<
			"\tSampling Frequency\t:\t" << altFormat.samplingrate << "\n" <<
			"\tBit depth\t\t:\t" << altFormat.bitdepth << "\n" << std::flush;
		level = CLog::ELogLevel::ASLOG_WARN;
		_destFormat = SetAudioFormatfromWFX(*pAltFormat);
	}
	else if (result == AUDCLNT_E_UNSUPPORTED_FORMAT && _shareMode == AUDCLNT_SHAREMODE_SHARED) {
		rmd_lunchFormat(outputStr);
		outputStr << "Use only sharemode" << std::flush;
		_destFormat = SetAudioFormatfromWFX(wfx);
		result = S_OK;
	}
	else {
		level = CLog::ELogLevel::ASLOG_ERROR;
		outputStr << "It is an unusable format" << std::flush;
	}
	CLog::Log(level, outputStr.str());
	return result;
}

IMMDevice* CWasapi::GetDefaultDevice(EDriverMode& _mode) {
	HRESULT result;
	IMMDeviceEnumerator* pDeviceEnum = nullptr;
	IMMDevice* pDevice = nullptr;

	result = CoCreateInstance(m_sCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, m_sIID_IMMDeviceEnumerator, (void**)&pDeviceEnum);
	if (!CheckError(result, __func__, "CoCreateInstance", "Success Create Enumerator", true))return nullptr;

	auto dataFlow = _mode == EDriverMode::AS_DRIVERMODE_RENDER ?
		EDataFlow::eRender : _mode == EDriverMode::AS_DRIVERMODE_CAPTURE ?
		EDataFlow::eCapture : EDataFlow::eAll;
	result = pDeviceEnum->GetDefaultAudioEndpoint(dataFlow, ERole::eConsole, &pDevice);
	if (!CheckError(result, __func__, "GetDefaultAudioEndpoint", "Success Get Default Audio Endpoint", true))return nullptr;

	IPropertyStore* pPropStore = nullptr;
	PROPVARIANT propVer{};
	pDevice->OpenPropertyStore(STGM_READ, &pPropStore);
	PropVariantInit(&propVer);
	pPropStore->GetValue(PKEY_Device_DeviceDesc, &propVer);
	std::string _m = _mode == EDriverMode::AS_DRIVERMODE_RENDER ?
		"Render" : _mode == EDriverMode::AS_DRIVERMODE_CAPTURE ?
		"Capture" : "Unknown";
	std::stringstream strstr;
	strstr << "Default Audio End Point" <<
		"\nDevice Name\t:\t" << Convert_Wstr_to_Str(std::wstring(propVer.bstrVal)) <<
		"\nDriver Name\t:\t" << CWasapi::GetDriverType() <<
		"\nDriver Mode\t:\t" << _m << std::flush;
	CLog::Log(CLog::ELogLevel::ASLOG_INFO, strstr.str());

	pDeviceEnum->Release();
	return pDevice;
}

WAVEFORMATEXTENSIBLE	SetWFXfromAudioFormat(AudioFormat& _info) {
	WAVEFORMATEXTENSIBLE wfx;
	SecureZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));

	wfx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wfx.Format.nChannels = _info.channel;
	wfx.Format.nSamplesPerSec = _info.samplingrate;
	wfx.Format.wBitsPerSample = _info.bitdepth;
	wfx.Format.nBlockAlign = (wfx.Format.wBitsPerSample / BITPERBYTE) * wfx.Format.nChannels;
	wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
	wfx.Samples.wValidBitsPerSample = _info.bitdepth;
	wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	return wfx;
}

AudioFormat	SetAudioFormatfromWFX(WAVEFORMATEXTENSIBLE& _WFX) {
	return AudioFormat(_WFX.Format.nSamplesPerSec, _WFX.Format.wBitsPerSample, _WFX.Format.nChannels);
}