#pragma once
template<class T>
class CSingleton {
public:
	CSingleton(const T*) = delete;						//�R�s�[�R���X�g���N�^�� delete �w��
	CSingleton& operator=(const CSingleton&) = delete;	//�R�s�[������Z�q�� delete �w��
	CSingleton(T&&) = delete;							//���[�u�R���X�g���N�^�� delete �w��
	CSingleton& operator=(CSingleton&&) = delete;		//���[�u������Z�q�� delete �w��

	static T& GetInstance() {
		static T inst;
		return inst;
	}

protected:
	CSingleton() = default;
	virtual ~CSingleton() = default;
};