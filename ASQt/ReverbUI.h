#pragma once

#include <QWidget>
#include "ui_ReverbUI.h"
#include "CReverb.h"

class ReverbUI : public QWidget {
	Q_OBJECT
public:
	ReverbUI(QWidget* parent = Q_NULLPTR);
	~ReverbUI();

	void Open(WPEffectBase _reverb);

private:
	std::unique_ptr<Ui::ReverbUI> m_ui;
	WPEffectBase m_Reverb;
private slots:
	void ChangeParam();
	void EnableSlot();
};
