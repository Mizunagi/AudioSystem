#include "ReverbWindow.h"

ReverbWindow::ReverbWindow(QWidget* parent) : QWidget(parent) {
}

ReverbWindow::~ReverbWindow() {
}

void ReverbWindow::Open(WPEffectBase reverb) {
	m_Reverb = reverb;
	m_RevWin = std::make_unique<Ui::ReverbUI>();
	m_RevWin->setupUi(this);

	this->connect(m_RevWin->DelayTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ReverbWindow::ChangeParam);
	this->connect(m_RevWin->DecayLevelSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ReverbWindow::ChangeParam);
	this->connect(m_RevWin->FeedbackSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ReverbWindow::ChangeParam);
	this->connect(m_RevWin->EnableCheckBox, &QCheckBox::stateChanged, this, &ReverbWindow::EnableSlot);

	if (auto rev = m_Reverb.lock()) {
		rev->SetEffectParam(ReverbParam(m_RevWin->DelayTimeSpinBox->value(), m_RevWin->DecayLevelSpinBox->value(), m_RevWin->FeedbackSpinBox->value()));
	}

	this->setWindowTitle("Reverb");
	this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
	this->show();
}

void ReverbWindow::ChangeParam() {
	if (auto rev = m_Reverb.lock()) {
		rev->SetEffectParam(ReverbParam(m_RevWin->DelayTimeSpinBox->value(), m_RevWin->DecayLevelSpinBox->value(), m_RevWin->FeedbackSpinBox->value()));
	}
}

void ReverbWindow::EnableSlot() {
	if (auto rev = m_Reverb.lock()) {
		rev->Enable(m_RevWin->EnableCheckBox->isChecked());
	}
}