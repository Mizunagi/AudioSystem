#include "ASQt.h"

ASQt::ASQt(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	Connect();
	ASSetup();
}

ASQt::~ASQt() {
	ASShutDown();
}

void ASQt::dragEnterEvent(QDragEnterEvent* e) {
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void ASQt::dropEvent(QDropEvent* e) {
	QList<QUrl> fileUrls = e->mimeData()->urls();
	std::wstring u = fileUrls.first().toLocalFile().toStdWString();
	if (CWavFile::CreateInstance(Convert_Wstr_to_Str(u), EHoldMode::WAVE_HOLDMODE_STREAM, m_Wave)) {
		Stop();
		m_Source->Bind(m_Wave);
		ui.PlayFileBrouser->setText(fileUrls.first().toLocalFile());
	}
}

void ASQt::Connect() {
	QWidget::connect(ui.PlayButton, &QPushButton::clicked, this, &ASQt::Play);
	QWidget::connect(ui.StopButton, &QPushButton::clicked, this, &ASQt::Stop);
	QWidget::connect(ui.PauseButton, &QPushButton::clicked, this, &ASQt::Pause);
	QWidget::connect(ui.actionReverb, &QAction::triggered, this, &ASQt::OpenRevWin);
	QWidget::connect(ui.actionEqualizer, &QAction::triggered, this, &ASQt::OpenEQWin);

	ui.StopButton->setEnabled(false);
	ui.PauseButton->setEnabled(false);
}

void ASQt::ASSetup() {
	AudioFormat altFormat;
	WasapiLunchInfo lunch(DeviceInfo("", CWasapi::GetDriverType(), EDriverMode::AS_DRIVERMODE_RENDER), AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE_SHARED, &altFormat);
	if (!CAudioSystem::GetInstance().LunchDevice(EDriverMode::AS_DRIVERMODE_RENDER, lunch))
		return;
	WasapiRenderSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST);
	if (!CAudioSystem::GetInstance().SetupDevice(EDriverMode::AS_DRIVERMODE_RENDER, setup))
		return;
	WasapiStartInfo start(2000);
	if (!CAudioSystem::GetInstance().Start(EDriverMode::AS_DRIVERMODE_RENDER, start))
		return;

	QString DeviceName = QString::fromStdString(CAudioSystem::GetInstance().GetLunchedDeviceInfo(EDriverMode::AS_DRIVERMODE_RENDER).deviceName);
	bool res = CAudioSystem::GetInstance().CreateMasterTrack(m_Master);
	res = CAudioSystem::GetInstance().CreateSourceTrack(EDriverMode::AS_DRIVERMODE_RENDER, 0, EEffectTiming::AS_EFFECTTIMING_SENDBUFFER, m_Source);

	res = CAudioSystem::GetInstance().CreateEffector(m_Source, m_Effector);
	if (auto eff = m_Effector.lock()) {
		eff->AddEffect<CReverb>(m_Reverb);
		auto rev = m_Reverb.lock();
		rev->SetEffectParam(ReverbParam(250.0f, 0.3f, 0.5f));
		rev->Enable(false);
		eff->AddEffect<CEqualizer>(m_Equalizer);
		auto eqlz = m_Equalizer.lock();
		eqlz->Enable(false);
	}
}

void ASQt::ASShutDown() {
	WasapiStopInfo stop;
	if (!CAudioSystem::GetInstance().Stop(EDriverMode::AS_DRIVERMODE_RENDER, stop))
		return;
}

void ASQt::Play() {
	if (m_Source->IsBinding()) {
		ui.StopButton->setEnabled(true);
		ui.PauseButton->setEnabled(true);
		ui.PlayButton->setEnabled(false);
		m_Source->Play(LOOP_INFINITY);
	}
}

void ASQt::Stop() {
	ui.PlayButton->setEnabled(true);
	ui.PauseButton->setEnabled(false);
	ui.StopButton->setEnabled(false);
	m_Source->Stop();
}

void ASQt::Pause() {
	ui.PlayButton->setEnabled(true);
	ui.StopButton->setEnabled(true);
	ui.PauseButton->setEnabled(false);
	m_Source->Pause();
}

void ASQt::OpenRevWin() {
	m_ReverbWindow = std::make_unique<ReverbWindow>(this);
	m_ReverbWindow->Open(m_Reverb);
}

void ASQt::OpenEQWin() {
	m_EqWindow = std::make_unique<EqualizerWindow>(this);
	m_EqWindow->Open(m_Equalizer);
}