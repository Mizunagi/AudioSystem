#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ASQt.h"
#include "ReverbWindow.h"
#include "EqualizerWindow.h"
#include "ui_Equalizer.h"
#include "CAudioSystem.h"
#include "CWasapi.h"
#include "CWavFile.h"
#include "CEqualizer.h"

#include <QUrl>
#include <QMimeData>
#include <QDropEvent>

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = Q_NULLPTR);
	~ASQt();

	void dragEnterEvent(QDragEnterEvent* e) override;
	void dropEvent(QDropEvent* e)override;

private:
	Ui::ASQtClass ui;
	std::unique_ptr<ReverbWindow> m_ReverbWindow;
	std::unique_ptr<EqualizerWindow> m_EqWindow;
	SPMasterTrack m_Master;
	SPSourceTrack m_Source;
	WPEffectManager m_Effector;
	WPEffectBase m_Reverb;
	WPEffectBase m_Equalizer;
	SPWavFile m_Wave;

	void Connect();
	void ASSetup();
	void ASShutDown();

private slots:
	void Play();
	void Stop();
	void Pause();
	void OpenRevWin();
	void OpenEQWin();
};
