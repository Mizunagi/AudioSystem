#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ASQt.h"

class ASQt : public QMainWindow {
	Q_OBJECT
public:
	ASQt(QWidget* parent = Q_NULLPTR);
private:
	Ui::ASQtClass ui;
};
