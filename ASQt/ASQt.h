#pragma once

#include <QtWidgets/QMainWindow>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QTextCodec>

#include "ui_ASQt.h"
#include "ReverbUI.h"
#include "EqualizerUI.h"
#include "CAudioSystem.h"
#include "CWasapi.h"
#include "CWavFile.h"

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = Q_NULLPTR);
	~ASQt();

	bool Open() {
		if (!ASSetup()) return false;

		m_ui.setupUi(this);
		Connect();
		show();
		return true;
	}

private:
	Ui::ASQtClass m_ui;
	std::unique_ptr<ReverbUI> m_ReverbUI;
	std::unique_ptr<EqualizerUI> m_EQUI;

	SPMasterTrack m_Master;
	SPSourceTrack m_Source;
	WPEffectManager m_Effector;
	WPEffectBase m_Reverb;
	WPEffectBase m_EQ;
	SPWavFile m_Wave;
	std::vector<std::string> m_FilePaths;

	void Connect();
	bool ASSetup();
	void ASShutDown();
	std::string utf16_to_sjis(const QString& src);
	QString sjis_to_utf16(const std::string& src);

protected:
	void dragEnterEvent(QDragEnterEvent* _e) override;
	void dropEvent(QDropEvent* _e) override;

private slots:
	void Play();
	void Stop();
	void Pause();
	void ChangeVol();
	void OpenReverbUI();
	void OpenEQUI();
	void OpenPreferenceUI();
};
