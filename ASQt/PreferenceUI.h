#pragma once

#include <QWidget>
#include "ui_PreferenceUI.h"

class PreferenceUI : public QWidget {
	Q_OBJECT
public:
	PreferenceUI(QWidget* parent = Q_NULLPTR);
	~PreferenceUI();
private:
	Ui::PreferenceUI ui;
};
