#pragma once
#include "ui_Equalizer.h"
#include "CEqualizer.h"
#include <QWidget>

class EqualizerWindow : public QWidget {
	Q_OBJECT
public:
	EqualizerWindow(QWidget* parent = Q_NULLPTR);
	~EqualizerWindow();

	void Open(WPEffectBase eq);

private:
	std::unique_ptr <Ui::EQUI> m_EqWin;
	WPEffectBase m_Eq;
	EQParam m_Param;
private slots:
	void ChangeParam();
	void EnableSlot();
};
