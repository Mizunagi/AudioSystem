#include "ReverbUI.h"

ReverbUI::ReverbUI(QWidget* parent) : QWidget(parent) {
}

ReverbUI::~ReverbUI()
{
}

void ReverbUI::Open(WPEffectBase _reverb) {
	if (!m_ui) {
		m_Reverb = _reverb;
		m_ui = std::make_unique<Ui::ReverbUI>();
		m_ui->setupUi(this);

		connect(m_ui->m_DelayTime, QOverload<int>::of(&QSpinBox::valueChanged), this, &ReverbUI::ChangeParam);
		connect(m_ui->m_DecayLevel, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ReverbUI::ChangeParam);
		connect(m_ui->m_FeedBack, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ReverbUI::ChangeParam);
		connect(m_ui->m_Enable, &QCheckBox::stateChanged, this, &ReverbUI::EnableSlot);

		if (auto rev = m_Reverb.lock()) {
			rev->SetEffectParam(ReverbParam(m_ui->m_DelayTime->value(), m_ui->m_DecayLevel->value(), m_ui->m_FeedBack->value()));
		}
	}

	setWindowTitle("Reverb");
	setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
	show();
}

void ReverbUI::ChangeParam() {
	if (auto rev = m_Reverb.lock()) {
		rev->SetEffectParam(ReverbParam(m_ui->m_DelayTime->value(), m_ui->m_DecayLevel->value(), m_ui->m_FeedBack->value()));
	}
}

void ReverbUI::EnableSlot() {
	if (auto rev = m_Reverb.lock()) {
		rev->Enable(m_ui->m_Enable->isChecked());
	}
}