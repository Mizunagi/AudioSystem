#pragma once
#include "ui_Reverb.h"
#include "CReverb.h"
#include <QWidget>
class ReverbWindow :public QWidget {
	Q_OBJECT
public:
	ReverbWindow(QWidget* parent = Q_NULLPTR);
	~ReverbWindow();

	void Open(WPEffectBase reverb);

private:
	std::unique_ptr<Ui::ReverbUI> m_RevWin;
	WPEffectBase m_Reverb;
private slots:
	void ChangeParam();
	void EnableSlot();
};
