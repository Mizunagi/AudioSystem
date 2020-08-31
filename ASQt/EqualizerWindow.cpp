#include "EqualizerWindow.h"

EqualizerWindow::EqualizerWindow(QWidget* parent) :QWidget(parent) {
}

EqualizerWindow::~EqualizerWindow() {
}

void EqualizerWindow::Open(WPEffectBase eq) {
	m_Eq = eq;

	m_EqWin = std::make_unique<Ui::EQUI>();
	m_EqWin->setupUi(this);

	this->connect(m_EqWin->eq55, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq77, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq110, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq156, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq220, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq311, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq440, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq622, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq880, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq1_2k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq1_8k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq2_5k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq3_5k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq5k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq7k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq10k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq14k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->eq20k, &QSlider::valueChanged, this, &EqualizerWindow::ChangeParam);
	this->connect(m_EqWin->EnableCheckBox, &QCheckBox::stateChanged, this, &EqualizerWindow::EnableSlot);

	m_Param.params.push_back(EQSingleParam(55.0f, 0.0f, 1.0f, m_EqWin->eq55->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(77.0f, 0.0f, 1.0f, m_EqWin->eq77->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(110.0f, 0.0f, 1.0f, m_EqWin->eq110->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(156.0f, 0.0f, 1.0f, m_EqWin->eq156->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(220.0f, 0.0f, 1.0f, m_EqWin->eq220->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(311.0f, 0.0f, 1.0f, m_EqWin->eq311->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(440.0f, 0.0f, 1.0f, m_EqWin->eq440->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(622.0f, 0.0f, 1.0f, m_EqWin->eq622->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(880.0f, 0.0f, 1.0f, m_EqWin->eq880->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(1200.0f, 0.0f, 1.0f, m_EqWin->eq1_2k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(1800.0f, 0.0f, 1.0f, m_EqWin->eq1_8k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(2500.0f, 0.0f, 1.0f, m_EqWin->eq2_5k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(3500.0f, 0.0f, 1.0f, m_EqWin->eq3_5k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(5000.0f, 0.0f, 1.0f, m_EqWin->eq5k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(7000.0f, 0.0f, 1.0f, m_EqWin->eq7k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(10000.0f, 0.0f, 1.0f, m_EqWin->eq10k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(14000.0f, 0.0f, 1.0f, m_EqWin->eq14k->value(), EQType::EQTYPE_PEAKING));
	m_Param.params.push_back(EQSingleParam(20000.0f, 0.0f, 1.0f, m_EqWin->eq20k->value(), EQType::EQTYPE_PEAKING));
	if (auto eq = m_Eq.lock()) {
		eq->SetEffectParam(m_Param);
	}

	this->setWindowTitle("Equalizer");
	this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
	this->show();
}

void EqualizerWindow::ChangeParam() {
	m_Param.params[0] = EQSingleParam(55.0f, 0.0f, 1.0f, m_EqWin->eq55->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[1] = EQSingleParam(77.0f, 0.0f, 1.0f, m_EqWin->eq77->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[2] = EQSingleParam(110.0f, 0.0f, 1.0f, m_EqWin->eq110->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[3] = EQSingleParam(156.0f, 0.0f, 1.0f, m_EqWin->eq156->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[4] = EQSingleParam(220.0f, 0.0f, 1.0f, m_EqWin->eq220->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[5] = EQSingleParam(311.0f, 0.0f, 1.0f, m_EqWin->eq311->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[6] = EQSingleParam(440.0f, 0.0f, 1.0f, m_EqWin->eq440->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[7] = EQSingleParam(622.0f, 0.0f, 1.0f, m_EqWin->eq622->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[8] = EQSingleParam(880.0f, 0.0f, 1.0f, m_EqWin->eq880->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[9] = EQSingleParam(1200.0f, 0.0f, 1.0f, m_EqWin->eq1_2k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[10] = EQSingleParam(1800.0f, 0.0f, 1.0f, m_EqWin->eq1_8k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[11] = EQSingleParam(2500.0f, 0.0f, 1.0f, m_EqWin->eq2_5k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[12] = EQSingleParam(3500.0f, 0.0f, 1.0f, m_EqWin->eq3_5k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[13] = EQSingleParam(5000.0f, 0.0f, 1.0f, m_EqWin->eq5k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[14] = EQSingleParam(7000.0f, 0.0f, 1.0f, m_EqWin->eq7k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[15] = EQSingleParam(10000.0f, 0.0f, 1.0f, m_EqWin->eq10k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[16] = EQSingleParam(14000.0f, 0.0f, 1.0f, m_EqWin->eq14k->value(), EQType::EQTYPE_PEAKING);
	m_Param.params[17] = EQSingleParam(20000.0f, 0.0f, 1.0f, m_EqWin->eq20k->value(), EQType::EQTYPE_PEAKING);
	if (auto eq = m_Eq.lock()) {
		eq->SetEffectParam(m_Param);
	}
}

void EqualizerWindow::EnableSlot() {
	if (auto eq = m_Eq.lock()) {
		eq->Enable(m_EqWin->EnableCheckBox->isChecked());
	}
}