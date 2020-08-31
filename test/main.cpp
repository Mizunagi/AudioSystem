/// <summary>
/// shared_ptrによる循環参照が発生している可能性があるためSourceTrackが正しく解放されずIOスレッドが終了されない->解決
///解決法
///weak_ptrを活用する(shared_ptrへの参照権を持つだけで所有権は持たない→shared_ptrのカウントに加味されない)
///
///エフェクト処理はスレッド分けせずにIOスレッドに行わせる→排他処理が一括で行えるため処理コストの低減
///＋ロック→解放→ロック→解放では途中で他所でロックが入るとエフェクトに余計な待ちが発生してしまうためドロップアウトの原因になりかねない
/// </summary>
/// <returns></returns>

//ドロップアウトらしきものの原因が不明->ソーストラックの音量演算で引数ミスがあった(ドロップアウトではなく音割れが起こってた)→解決

#include <CAudioSystem.h>
#include <CWavFile.h>
#include <CWasapi.h>
#include <CReverb.h>

int main() {
	bool result = false;
	DeviceList devList;
	uint32_t cnt_dev = 0;
	cnt_dev = CAudioSystem::GetInstance().GetDeviceList<CWasapi>(EDriverMode::AS_DRIVERMODE_RENDER, devList);
	uint32_t selectDevice = 0;

	for (uint32_t i = 0; i < cnt_dev; ++i) {
		std::cout << i << " : " << devList[i].deviceName << std::endl;
	}
	std::cout << ">" << std::endl;
	std::cin >> selectDevice;

	SPWavFile wav;
	result = CWavFile::CreateInstance("../Sounds/1-01 Teriqma.wav", EHoldMode::WAVE_HOLDMODE_STREAM, wav);
	if (!result)return 0;

	AudioFormat altFormat;
	WasapiLunchInfo lunch(devList[selectDevice], AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE_SHARED, &altFormat);
	result = CAudioSystem::GetInstance().LunchDevice(EDriverMode::AS_DRIVERMODE_RENDER, lunch);
	if (!result)return 0;

	WasapiRenderSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST);
	result = CAudioSystem::GetInstance().SetupDevice(EDriverMode::AS_DRIVERMODE_RENDER, setup);
	if (!result)return 0;

	SPMasterTrack master;
	result = CAudioSystem::GetInstance().CreateMasterTrack(master);
	if (!result)return 0;

	SPSourceTrack source;
	result = CAudioSystem::GetInstance().CreateSourceTrack(EDriverMode::AS_DRIVERMODE_RENDER, 0, EEffectTiming::AS_EFFECTTIMING_SENDBUFFER, source);
	if (!result)return 0;

	source->Bind(wav);

	WPEffectManager effector;
	result = CAudioSystem::GetInstance().CreateEffector(std::static_pointer_cast<CTrackBase>(source), effector);
	if (!result)return 0;

	WPEffectBase reverb;
	if (auto eff = effector.lock()) {
		eff->AddEffect<CReverb>(reverb);
		auto rev = reverb.lock();
		ReverbParam revParam(1000.0f, 0.8f, 0.8f);
		rev->SetEffectParam(revParam);
		rev->Enable(false);
	}

	WasapiStartInfo start(2000);
	result = CAudioSystem::GetInstance().Start(EDriverMode::AS_DRIVERMODE_RENDER, start);
	if (!result)return 0;

	source->Play(LOOP_INFINITY);

	while (true) {
		if (GetAsyncKeyState((VK_ESCAPE)))break;
		if (GetAsyncKeyState(VK_F1)) {
			if (auto rev = reverb.lock()) {
				rev->Enable(true);
			}
		}
		if (GetAsyncKeyState(VK_F2)) {
			if (auto rev = reverb.lock()) {
				rev->Enable(false);
			}
		}
	}

	source->Stop();

	WasapiStopInfo stop;
	result = CAudioSystem::GetInstance().Stop(EDriverMode::AS_DRIVERMODE_RENDER, stop);
	if (!result)return 0;

	/*for (uint32_t y = 0; y < renderBuffer.sizeY(); ++y) {
		for (uint32_t x = 0; x < renderFrames; ++x) {
			renderBuffer[y][x] = amplitude * std::sin(2.0f * M_PI * baseFreq * x / pbase->GetFormat().samplingrate);
		}
	}*/

	return 0;
}

//int main() {
//	CWasapi wasapi;
//
//	SPWavFile wav;
//	if (!CWavFile::CreateInstance("1-01 Teriqma.wav", EHoldMode::WAVE_HOLDMODE_STREAM, wav))return 0;
//
//	uint32_t cnt_dev = 0;
//	uint32_t selectDevice = 0;
//	DeviceList list;
//	cnt_dev = wasapi.GetDeviceList(EDriverMode::AS_DRIVERMODE_RENDER, list);
//	for (uint32_t i = 0; auto dev : list) {
//		std::cout << i << " : " << list[i++].deviceName << std::endl;
//	}
//	std::cout << ">" << std::endl;
//	std::cin >> selectDevice;
//
//	AudioFormat altFormat;
//	WasapiLunchInfo lunch(list[selectDevice], AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE_SHARED, &altFormat);
//	if (!wasapi.LunchDevice(lunch))return 0;
//
//	WasapiRenderSetupInfo renderSetup(0, AUDCLNT_STREAMFLAGS_NOPERSIST);
//	if (!wasapi.SetupDevice(renderSetup))return 0;
//
//	WasapiStartInfo start(2000);
//	if (!wasapi.Start(start))return 0;
//
//	uint32_t  srcPoint = 0;
//	CLineBuffer<float> buf;
//	while (!GetAsyncKeyState(VK_ESCAPE)) {
//		if (!wasapi.WaitForProcess()) {
//			break;
//		}
//
//		FramesInfo frames;
//		wasapi.GetFrames(frames);
//		if (buf.empty()) {
//			buf.resize(wasapi.GetFormat().channel, frames.maxFrameSize);
//		}
//
//		uint32_t renderframes = frames.paddingFrameSize != NOPADDING ? frames.paddingFrameSize : frames.maxFrameSize;
//		wav->GetBuffer(buf, 0, srcPoint, renderframes);
//		wasapi.Process(buf, renderframes);
//		srcPoint += renderframes;
//	}
//
//	/// <summary>
//	/// CWasapiには異常なし→SourceTrackかMasterTrackに異常あり
//	/// </summary>
//	/// <returns></returns>
//
//	WasapiStopInfo stop;
//	if (!wasapi.Stop(stop))return 0;
//
//	return 0;
//}