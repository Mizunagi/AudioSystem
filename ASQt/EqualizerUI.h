#pragma once

#include <QWidget>
#include "ui_EqualizerUI.h"

class EqualizerUI : public QWidget {
	Q_OBJECT
public:
	EqualizerUI(QWidget* parent = Q_NULLPTR);
	~EqualizerUI();
private:
	Ui::EqualizerUI ui;
};
