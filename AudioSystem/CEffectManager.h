#pragma once
#include "AudioDefine.h"
#include "CStdPtrHelper.h"
#include "CEffectBase.h"

class CEffectManager;
using SPEffectManager = std::shared_ptr<CEffectManager>;
using WPEffectManager = std::weak_ptr<CEffectManager>;

class CEffectManager {
public:
	friend class CAudioSystem;
	template<typename T>
	friend class CStdPtrHelper;

	template<class EFFECT>
	void AddEffect(WPEffectBase& _effect);
	void Execute(CLineBuffer<float>& _buffer, uint32_t _rednerFrames);
	const size_t GetEffectSize() { return m_Effects.size(); }
	void Flush();

private:
	CEffectManager(AudioFormat _format);
	~CEffectManager();

	static SPEffectManager CreateInstance(AudioFormat _format) { return CStdPtrHelper<CEffectManager>::make_shared(_format); }

	std::vector<SPEffectBase> m_Effects;
	AudioFormat m_Format;
};

template<class EFFECT>
void CEffectManager::AddEffect(WPEffectBase& _effect) {
	//参考資料(なぜヘッダに実装を書いた？)
	//https://qiita.com/i153/items/38f9688a9c80b2cb7da7
	static_assert(std::is_base_of<CEffectBase, EFFECT>::value, "The specified type is not a derived type of CEffectBase");
	SPEffectBase instance = std::make_shared<EFFECT>(m_Format);
	_effect = instance;
	m_Effects.push_back(std::move(instance));
}
