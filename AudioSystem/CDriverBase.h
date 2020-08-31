#ifndef _CDRIVERBASE_
#define _CDRIVERBASE_
#include "AudioDefine.h"

enum class EDriverMode {
	AS_DRIVERMODE_NONE = 0,
	AS_DRIVERMODE_RENDER,
	AS_DRIVERMODE_CAPTURE
};

enum class EDriverState {
	AS_DRIVERSTATE_NONE = 0,
	AS_DRIVERSTATE_LUNCHED,
	AS_DRIVERSTATE_RENDERING,
	AS_DRIVERSTATE_CAPTURING,
	AS_DRIVERSTATE_STOPPING
};

struct DeviceInfo {
	DeviceInfo(std::string _device, std::string _driver, EDriverMode _mode) :deviceName(_device), driverName(_driver), driverMode(_mode) {}
	std::string deviceName;
	std::string driverName;
	EDriverMode driverMode = EDriverMode::AS_DRIVERMODE_NONE;
};
using DeviceList = std::vector<DeviceInfo>;

struct LunchInfo {
	LunchInfo(DeviceInfo& _device, AudioFormat& _format) :lunchDevice(_device), lunchFormat(_format) {}
	DeviceInfo lunchDevice;
	AudioFormat lunchFormat;
};

struct SetupInfo {
};

struct StartInfo {
};

struct StopInfo {
};

struct FramesInfo {
	uint32_t maxFrameSize = 0;
	int32_t paddingFrameSize = 0;
};

class CDriverBase {
public:
	CDriverBase() {}
	virtual ~CDriverBase() {}

	virtual uint32_t GetDeviceList(EDriverMode _mode, DeviceList& _destList) = 0;
	virtual bool LunchDevice(LunchInfo& _info) = 0;
	virtual bool SetupDevice(SetupInfo& _info) = 0;
	virtual bool Start(StartInfo& _info) = 0;
	virtual bool Stop(StopInfo& _info) = 0;

	virtual bool WaitForProcess() = 0;
	virtual bool GetFrames(FramesInfo& _info) = 0;
	virtual int32_t Process(CLineBuffer<float>& outputBuffer, uint32_t renderFrames) = 0;
	AudioFormat GetFormat() { return m_Format; }
	EDriverState GetState() { return m_DriverState; }
	virtual DeviceInfo GetLunchedDeviceInfo() = 0;
protected:

	EDriverState m_DriverState = EDriverState::AS_DRIVERSTATE_NONE;
	AudioFormat m_Format;
};
#endif