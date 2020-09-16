#include "ASQt.h"

ASQt::ASQt(QWidget* parent) : QMainWindow(parent) {}

ASQt::~ASQt() {
	ASShutDown();
}

void ASQt::Connect() {
	connect(m_ui.m_PlayButton, &QPushButton::clicked, this, &ASQt::Play);
	connect(m_ui.m_StopButton, &QPushButton::clicked, this, &ASQt::Stop);
	connect(m_ui.m_PauseButton, &QPushButton::clicked, this, &ASQt::Pause);
	connect(m_ui.m_VolumeSlider, &QSlider::sliderReleased, this, &ASQt::ChangeVol);
	connect(m_ui.actionReverb, &QAction::triggered, this, &ASQt::OpenReverbUI);
	connect(m_ui.actionEqualizer, &QAction::triggered, this, &ASQt::OpenEQUI);
	connect(m_ui.m_PlayList, &QListWidget::doubleClicked, this, &ASQt::Play);
	//connect(m_ui., &QAction::triggered, this, &ASQt::OpenEQUI);
}

bool ASQt::ASSetup() {
	AudioFormat altFormat;
	WasapiLunchInfo lunch(DeviceInfo("", CWasapi::GetDriverType(), EDriverMode::AS_DRIVERMODE_RENDER), AudioFormat(48000, 16, 2), AUDCLNT_SHAREMODE_SHARED, &altFormat);
	if (!CAudioSystem::GetInstance().LunchDevice(EDriverMode::AS_DRIVERMODE_RENDER, lunch)) {
		return false;
	}
	WasapiRenderSetupInfo setup(0, AUDCLNT_STREAMFLAGS_NOPERSIST);
	if (!CAudioSystem::GetInstance().SetupDevice(EDriverMode::AS_DRIVERMODE_RENDER, setup)) {
		close();
		return false;
	}
	WasapiStartInfo start(2000);
	if (!CAudioSystem::GetInstance().Start(EDriverMode::AS_DRIVERMODE_RENDER, start)) {
		close();
		return false;
	}

	bool res = CAudioSystem::GetInstance().CreateMasterTrack(m_Master);
	res = CAudioSystem::GetInstance().CreateSourceTrack(EDriverMode::AS_DRIVERMODE_RENDER, 0, EEffectTiming::AS_EFFECTTIMING_IO, m_Source);

	res = CAudioSystem::GetInstance().CreateEffector(m_Source, m_Effector);
	if (auto eff = m_Effector.lock()) {
		eff->AddEffect<CReverb>(m_Reverb);
		auto rev = m_Reverb.lock();
		rev->SetEffectParam(ReverbParam(250.0f, 0.3f, 0.5f));
		rev->Enable(false);
		eff->AddEffect<CEqualizer>(m_EQ);
		auto eqlz = m_EQ.lock();
		eqlz->Enable(false);
	}
	return true;
}

void ASQt::ASShutDown() {
	WasapiStopInfo stop;
	if (!CAudioSystem::GetInstance().Stop(EDriverMode::AS_DRIVERMODE_RENDER, stop))
		return;

	CAudioSystem::GetInstance().Release(EDriverMode::AS_DRIVERMODE_RENDER);
}

std::string ASQt::utf16_to_sjis(const QString& src) {
	QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
	QByteArray encoded = codec->fromUnicode(src);
	return encoded.toStdString();
}

QString ASQt::sjis_to_utf16(const std::string& src) {
	QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
	return codec->toUnicode(src.c_str());
}

void ASQt::dragEnterEvent(QDragEnterEvent* _e) {
	if (_e->mimeData()->hasUrls()) {
		_e->acceptProposedAction();
	}
}

void ASQt::dropEvent(QDropEvent* _e) {
	Stop();
	m_ui.m_PlayList->clear();
	m_FilePaths.clear();
	m_FilePaths.shrink_to_fit();
	QList<QUrl> fileUrls = _e->mimeData()->urls();
	for (uint32_t i = 0; i < fileUrls.size(); ++i) {
		m_FilePaths.push_back(utf16_to_sjis(fileUrls[i].toLocalFile()));
		size_t pos = m_FilePaths[i].find_last_of("//");
		m_ui.m_PlayList->addItem(sjis_to_utf16(m_FilePaths[i].substr(pos + 1)));
	}
}

void ASQt::Play() {
	Stop();
	m_Source->If_Stop([this]() {
		int currentRow = m_ui.m_PlayList->currentRow();
		auto listSize = m_ui.m_PlayList->contentsSize();

		if (++currentRow) {
			m_ui.m_PlayList->setCurrentRow(currentRow);
			if (CWavFile::CreateInstance(m_FilePaths[currentRow], EHoldMode::WAVE_HOLDMODE_STREAM, m_Wave)) {
				m_ui.m_StopButton->setEnabled(true);
				m_ui.m_PauseButton->setEnabled(true);
				m_ui.m_PlayButton->setEnabled(false);
				m_Source->Bind(m_Wave);
				m_Source->Play();
			}
		}
		else {
			m_ui.m_PlayButton->setEnabled(true);
			m_ui.m_PauseButton->setEnabled(false);
			m_ui.m_StopButton->setEnabled(false);
		}
		});

	int playIdx = m_ui.m_PlayList->currentRow();
	if (CWavFile::CreateInstance(m_FilePaths[playIdx], EHoldMode::WAVE_HOLDMODE_STREAM, m_Wave)) {
		m_Source->Bind(m_Wave);

		m_ui.m_StopButton->setEnabled(true);
		m_ui.m_PauseButton->setEnabled(true);
		m_ui.m_PlayButton->setEnabled(false);
		m_Source->Play();
	}
}

void ASQt::Stop() {
	m_ui.m_PlayButton->setEnabled(true);
	m_ui.m_PauseButton->setEnabled(false);
	m_ui.m_StopButton->setEnabled(false);
	m_Source->Stop();
}

void ASQt::Pause() {
	m_ui.m_PlayButton->setEnabled(true);
	m_ui.m_StopButton->setEnabled(true);
	m_ui.m_PauseButton->setEnabled(false);
	m_Source->Pause();
}

void ASQt::ChangeVol() {
	m_Source->Volume(static_cast<float>(m_ui.m_VolumeSlider->value()) / 100.0f);
}

void ASQt::OpenReverbUI() {
	if (!m_ReverbUI)
		m_ReverbUI = std::make_unique<ReverbUI>(this);
	m_ReverbUI->Open(m_Reverb);
}

void ASQt::OpenEQUI() {
	if (!m_EQUI)
		m_EQUI = std::make_unique<EqualizerUI>(this);
	m_EQUI->Open(m_EQ);
}

void ASQt::OpenPreferenceUI() {
}