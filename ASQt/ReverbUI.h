#pragma once

#include <QWidget>
#include "ui_ReverbUI.h"

class ReverbUI : public QWidget {
	Q_OBJECT
public:
	ReverbUI(QWidget* parent = Q_NULLPTR);
	~ReverbUI();
private:
	Ui::ReverbUI ui;
};
