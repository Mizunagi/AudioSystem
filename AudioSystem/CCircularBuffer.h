//*****************************************************************************
//!	@file	:CCircularBuffer.h
//!	@brief	:�����O�o�b�t�@
//!	@note	:
//!	@author	:
//*****************************************************************************
#ifndef _CCIRCULARBUFFER_
#define _CCIRCULARBUFFER_

#include "AudioDefine.h"

//�Q�l
//http://precure-3dprinter.hatenablog.jp/entry/2018/02/12/%E4%BD%BF%E3%81%86%E3%81%AE%E3%81%8B%E5%88%86%E3%81%8B%E3%82%89%E3%81%AA%E3%81%84%E3%83%AA%E3%83%B3%E3%82%B0%E3%83%90%E3%83%83%E3%83%95%E3%82%A1%E3%81%AE%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9
//https://mickey24.hatenablog.com/entry/20081021/1224590753
	//=============================================================================
	//!	@class	:CCircularBuffer
	//!	@brief	:�����O�o�b�t�@
	//=============================================================================
template<typename T>
class CCircularBuffer {
public:
	CCircularBuffer();
	explicit CCircularBuffer(size_t _n);
	CCircularBuffer(const CCircularBuffer<T>& _right);
	~CCircularBuffer();
	T operator [](int32_t _idx)const;
	T& operator [](int32_t _idx);
	operator const T* ()const;
	CCircularBuffer<T>& operator =(const CCircularBuffer<T>& _right);
	CCircularBuffer<T>& operator ++();
	CCircularBuffer<T> operator ++(int);
	CCircularBuffer<T>& operator --();
	CCircularBuffer<T> operator --(int);

	void resize(size_t _n);
	void copy(const CCircularBuffer<T>& _right);
	void clear();
	void set(const int32_t _idx);
	bool empty();
	void reverse();
	size_t size()const;

private:
	std::unique_ptr<T[]> m_Buffer = nullptr;
	size_t m_size = 0;
	size_t m_Realsize = 0;
	uint32_t m_Beg = 0;
	bool m_reverse = false;

	void create(size_t n);
	int32_t index(int32_t idx);
};

template<typename T>
CCircularBuffer<T>::CCircularBuffer() {
};

template<typename T>
CCircularBuffer<T>::CCircularBuffer(size_t _n) {
	create(_n);
}

template<typename T>
CCircularBuffer<T>::CCircularBuffer(const CCircularBuffer<T>& _right) {
	copy(_right);
}

template<typename T>
CCircularBuffer<T>::~CCircularBuffer() {
};

template<typename T>
CCircularBuffer<T>::operator const T* ()const {
	return m_Buffer;
}

//=============================================================================
//!	@fn		:operator []
//!	@brief	:�N�_����������̗v�f�����
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
T CCircularBuffer<T>::operator [](int32_t _idx)const {
	return m_Buffer[index(_idx)];
}

//=============================================================================
//!	@fn		:operator []
//!	@brief	:�N�_����������̗v�f�����
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
T& CCircularBuffer<T>::operator [](int32_t _idx) {
	return m_Buffer[index(_idx)];
}

template<typename T>
CCircularBuffer<T>& CCircularBuffer<T>::operator =(const CCircularBuffer<T>& right) {
	copy(right); return *this;
}

//=============================================================================
//!	@fn		:operator --
//!	@brief	:�擪��1�O��
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
CCircularBuffer<T>& CCircularBuffer<T>::operator ++() {
	set(1);
	return *this;
}

//=============================================================================
//!	@fn		:operator --
//!	@brief	:�擪��1�O��
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
CCircularBuffer<T> CCircularBuffer<T>::operator ++(int) {
	auto t = *this;
	set(1);
	return t;
}

//=============================================================================
//!	@fn		:operator --
//!	@brief	:�擪��1����
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
CCircularBuffer<T>& CCircularBuffer<T>::operator --() {
	set(-1);
	return *this;
}

//=============================================================================
//!	@fn		:operator --
//!	@brief	:�擪��1����
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
CCircularBuffer<T> CCircularBuffer<T>::operator --(int) {
	auto t = *this;
	set(-1);
	return t;
}

//=============================================================================
//!	@fn		:resize
//!	@brief	:�o�b�t�@���T�C�Y
//!	@param	:size_t �Đ����v�f��
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
void CCircularBuffer<T>::resize(size_t _n) {
	m_Buffer.reset();
	create(_n);
}

//=============================================================================
//!	@fn		:create
//!	@brief	:�o�b�t�@����
//!	@param	:size_t �����v�f��
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
void CCircularBuffer<T>::create(size_t _n) {
	int32_t mod = _n % 8;
	m_Realsize = _n + mod;
	m_size = _n;
	m_Buffer = std::make_unique<T[]>(_n);

	clear();
}

//=============================================================================
//!	@fn		:index
//!	@brief	:�C���f�b�N�X����
//!	@param	:int32_t �ړ���
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
int32_t CCircularBuffer<T>::index(int32_t _idx) {
	int32_t idx = m_reverse ? -(_idx + 1) : _idx;
	return m_Beg + idx >= 0 ? (m_Beg + idx) % m_size : (m_Beg + (m_size - std::abs(idx) % m_size)) % m_size;
}

//=============================================================================
//!	@fn		:set
//!	@brief	:�o�b�t�@�擪�ړ�
//!	@param	:
//!	@retval	:
//!	@note	:���݂̋N�_����������ړ�
//=============================================================================
template<typename T>
void CCircularBuffer<T>::set(const int32_t _idx) {
	m_Beg = index(_idx);
}

//=============================================================================
//!	@fn		:empty
//!	@brief	:�o�b�t�@��m�F
//!	@param	:true �o�b�t�@�� false �o�b�t�@���
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
bool CCircularBuffer<T>::empty() {
	return m_Buffer == nullptr ? true : false;
}

//=============================================================================
//!	@fn		:reverse
//!	@brief	:�����O��]�����t�]
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
void CCircularBuffer<T>::reverse() {
	m_reverse != m_reverse;
}

//=============================================================================
//!	@fn		:size
//!	@brief	:�o�b�t�@�T�C�Y�Q��
//!	@param	:
//!	@retval	:size_t �o�b�t�@�T�C�Y(�z��)
//!	@note	:
//=============================================================================
template<typename T>
size_t CCircularBuffer<T>::size()const {
	return m_size;
}

//=============================================================================
//!	@fn		:copy
//!	@brief	:�R�s�[
//!	@param	:const CCircularBuffer<T>& �R�s�[��
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
void CCircularBuffer<T>::copy(const CCircularBuffer<T>& _right) {
	if (m_Buffer) {
		m_Buffer.reset();
	}
	if (!_right.empty()) {
		create(_right.m_Realsize);
		m_Beg = _right.m_Beg;
		m_size = _right.m_size;
		m_Realsize = _right.m_Realsize;
		m_reverse = _right.m_reverse;
		memcpy(m_Buffer, _right.m_Buffer, sizeof(T) * m_Realsize);
	}
}

//=============================================================================
//!	@fn		:clear
//!	@brief	:�o�b�t�@�N���A
//!	@param	:
//!	@retval	:
//!	@note	:
//=============================================================================
template<typename T>
void CCircularBuffer<T>::clear() {
	m_Beg = 0;
	m_reverse = false;
	memset(m_Buffer.get(), NULL, sizeof(T) * m_Realsize);
}

#endif
//******************************************************************************
//	End of file.
//******************************************************************************