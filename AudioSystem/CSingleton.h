#pragma once
template<class T>
class CSingleton {
public:
	CSingleton(const T*) = delete;						//コピーコンストラクタを delete 指定
	CSingleton& operator=(const CSingleton&) = delete;	//コピー代入演算子も delete 指定
	CSingleton(T&&) = delete;							//ムーブコンストラクタを delete 指定
	CSingleton& operator=(CSingleton&&) = delete;		//ムーブ代入演算子も delete 指定

	static T& GetInstance() {
		static T inst;
		return inst;
	}

protected:
	CSingleton() = default;
	virtual ~CSingleton() = default;
};