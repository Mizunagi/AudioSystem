#include "CEffectManager.h"

CEffectManager::CEffectManager(AudioFormat _format) : m_Format(_format) {
}

CEffectManager::~CEffectManager() {
}

void CEffectManager::Execute(CLineBuffer<float>& _buffer, uint32_t _renderFrames) {
	for (auto& effect : m_Effects) {
		effect->Process(_buffer, _renderFrames);
	}
}

void CEffectManager::Flush() {
	for (auto& effect : m_Effects) {
		effect->Flush();
	}
}