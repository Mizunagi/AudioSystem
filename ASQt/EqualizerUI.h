#pragma once

#include <QWidget>
#include "ui_EqualizerUI.h"
#include "CEqualizer.h"

class EqualizerUI : public QWidget {
	Q_OBJECT
public:
	EqualizerUI(QWidget* parent = Q_NULLPTR);
	~EqualizerUI();

	void Open(WPEffectBase _eq);
private:
	std::unique_ptr<Ui::EqualizerUI> m_ui;
	WPEffectBase m_Eq;
	EQParam m_Param;
private slots:
	void ChangeParam();
	void EnableSlot();
};
