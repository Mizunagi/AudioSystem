#pragma once
#include "Utilities.h"

//参考(というか写経)https://qiita.com/Gaccho/items/e936de237676120aa8a0
template<typename T>
class CLineBuffer_XY {
private:
	//ここに2次元配列最終的な配列番号に見立てた1次元配列の座標が入る
	const size_t m_id_xy = 0;
	//1次元配列の参照(CLineBuffer.tの参照)
	const std::unique_ptr<T[]>& m_arr_ref;
public:
	constexpr CLineBuffer_XY<T>(const size_t _id, const std::unique_ptr<T[]>& _arr)
		: m_id_xy(_id), m_arr_ref(_arr) {}
	constexpr T operator+() const { return +m_arr_ref[m_id_xy]; }
	constexpr T operator-() const { return -m_arr_ref[m_id_xy]; }
	constexpr T operator+(const T& _a) const { return m_arr_ref[m_id_xy] + _a; }
	constexpr T operator-(const T& _a) const { return m_arr_ref[m_id_xy] - _a; }
	constexpr T operator*(const T& _a) const { return m_arr_ref[m_id_xy] * _a; }
	constexpr T operator/(const T& _a) const { return m_arr_ref[m_id_xy] / _a; }
	constexpr T operator%(const T& _a) const { return m_arr_ref[m_id_xy] % _a; }
	constexpr T operator&(const T& _a) const { return m_arr_ref[m_id_xy] & _a; }
	constexpr T operator|(const T& _a) const { return m_arr_ref[m_id_xy] | _a; }
	constexpr T operator^(const T& _a) const { return m_arr_ref[m_id_xy] ^ _a; }
	constexpr T operator=(const T& _a) const { return m_arr_ref[m_id_xy] = _a; }
	constexpr T operator+=(const T& _a) const { return m_arr_ref[m_id_xy] += _a; }
	constexpr T operator-=(const T& _a) const { return m_arr_ref[m_id_xy] -= _a; }
	constexpr T operator*=(const T& _a) const { return m_arr_ref[m_id_xy] *= _a; }
	constexpr T operator/=(const T& _a) const { return m_arr_ref[m_id_xy] /= _a; }
	constexpr T operator%=(const T& _a) const { return m_arr_ref[m_id_xy] %= _a; }
	constexpr T operator^=(const T& _a) const { return m_arr_ref[m_id_xy] ^= _a; }
	constexpr T operator|=(const T& _a) const { return m_arr_ref[m_id_xy] |= _a; }
	constexpr T operator&=(const T& _a) const { return m_arr_ref[m_id_xy] &= _a; }
	constexpr T operator++() const { return ++m_arr_ref[m_id_xy]; }
	constexpr T operator++(int) const { return m_arr_ref[m_id_xy]++; }
	constexpr T operator--() const { return --m_arr_ref[m_id_xy]; }
	constexpr T operator--(int) const { return m_arr_ref[m_id_xy]--; }
	constexpr T* operator&()const { return &m_arr_ref[m_id_xy]; }
	constexpr const bool operator!() const { return !m_arr_ref[m_id_xy]; }
	constexpr const bool operator&&(const T& _a) const { return (m_arr_ref[m_id_xy] && _a); }
	constexpr const bool operator||(const T& _a) const { return (m_arr_ref[m_id_xy] || _a); }
	constexpr const bool operator<(const T& _a) const { return (m_arr_ref[m_id_xy] < _a); }
	constexpr const bool operator>(const T& _a) const { return (m_arr_ref[m_id_xy] > _a); }
	constexpr const bool operator<=(const T& _a) const { return (m_arr_ref[m_id_xy] <= _a); }
	constexpr const bool operator>=(const T& _a) const { return (m_arr_ref[m_id_xy] >= _a); }
	constexpr const bool operator!=(const T& _a) const { return (m_arr_ref[m_id_xy] != _a); }
	constexpr const bool operator==(const T& _a) const { return (m_arr_ref[m_id_xy] == _a); }

	operator T() const { return m_arr_ref[m_id_xy]; }
};

template<typename T>
class CLineBuffer_X {
private:
	//ここには2次元配列のx軸先頭に見立てた1次元配列の座標が入る
	const size_t m_id_x = 0;
	//1次元配列の参照(CLineBuffer.tの参照)
	const std::unique_ptr<T[]>& m_arr_ref;
public:
	constexpr CLineBuffer_X<T>(const size_t _id, const std::unique_ptr<T[]>& _arr) : m_id_x(_id), m_arr_ref(_arr) {}
	//(2つ目の[])
	constexpr const CLineBuffer_XY<T> operator[](const size_t _x) const {
		const CLineBuffer_XY<T> arr_xy(m_id_x + _x, m_arr_ref);
		return arr_xy;
	}
	constexpr const CLineBuffer_XY<T> at(const size_t _x) const {
		const CLineBuffer_XY<T> arr_xy(m_id_x + _x, m_arr_ref);
		return arr_xy;
	}
	constexpr const CLineBuffer_XY<T> front() const {
		const CLineBuffer_XY<T> arr_xy(m_id_x, m_arr_ref);
		return arr_xy;
	}
};

template<typename T>
class CLineBuffer {
private:
	std::unique_ptr<T[]> m_arr_real;
	size_t m_Size_x = 0, m_Size_y = 0, m_RealSize_x = 0;
public:
	CLineBuffer<T>() { }
	//x*yの1次元配列を生成してすべての配列をvalueにする
	CLineBuffer<T>(const size_t _y, const size_t _x, const T _value)
		//_m256アライメントのためx軸方向のフレームは設定フレーム数より多い32の倍数で設定
		: m_RealSize_x(_x + (_x % 32)), m_Size_x((_x == 0) ? 1 : _x), m_Size_y((_y == 0) ? 1 : _y), m_arr_real(new T[m_RealSize_x * m_Size_y]) {
		const size_t xy = _x * _y;
		std::memset(m_arr_real.get(), _value, sizeof(float) * m_RealSize_x * m_Size_y);
	}
	//x*yの1次元配列を生成するだけ
	constexpr CLineBuffer<T>(const size_t _y, const size_t _x)
		: m_RealSize_x(_x + (_x % 32)), m_Size_x((_x == 0) ? 1 : _x), m_Size_y((_y == 0) ? 1 : _y), m_arr_real(new T[m_RealSize_x * m_Size_y]) {}
	//2次元配列に見立てた1次元配列のx軸の先頭のCLineBuffer_Xを入手(1つ目の[])
	constexpr const CLineBuffer_X<T> operator[](const size_t _y) const {
		CLineBuffer_X<T> arr_x(_y * m_Size_x, m_arr_real);
		return arr_x;
	}
	constexpr const CLineBuffer<T>& operator =(const CLineBuffer<T>& src) {
		m_arr_real.reset();
		m_arr_real = std::move(std::unique_ptr<T[]>(new T[src.m_Size_x * src.m_Size_y]));
		m_Size_x = src.m_Size_x; m_Size_y = src.m_Size_y;
		memcpy_s(m_arr_real.get(), sizeof(T) * (m_Size_x * m_Size_y), src.m_arr_real.get(), sizeof(T) * (src.m_Size_x * src.m_Size_y));
		return *this;
	}
	constexpr const CLineBuffer_X<T> at(const size_t _y) const {
		CLineBuffer_X<T> arr_x(_y * m_Size_x, m_arr_real);
		return arr_x;
	}
	constexpr const CLineBuffer_X<T> front() const {
		CLineBuffer_X<T> arr_x(0, m_arr_real);
		return arr_x;
	}
	std::unique_ptr<T[]> data() const { return m_arr_real; }
	constexpr size_t size() const { return m_Size_x * m_Size_y; }
	constexpr size_t realSize() const { return m_RealSize_x * m_Size_y; }
	constexpr size_t sizeX() const { return m_Size_x; }
	constexpr size_t sizeRealX() const { return m_RealSize_x; }
	constexpr size_t sizeY() const { return m_Size_y; }
	void resize(const size_t _y, const size_t _x) {
		m_arr_real.reset();
		m_RealSize_x = _x + (_x % 32);
		m_Size_x = _x == 0 ? 1 : _x;
		m_Size_y = _y == 0 ? 1 : _y;
		m_arr_real = std::make_unique<T[]>(m_Size_y * m_RealSize_x);
	}
	void zeroclear() {
		std::memset(m_arr_real.get(), NULL, sizeof(float) * m_RealSize_x * m_Size_y);
	}
	bool empty() { return m_arr_real ? false : true; }

	constexpr T* operator&() const { return m_arr_real.get(); }
	constexpr T* const get() const { return m_arr_real.get(); }
	constexpr T* const begin() const { return m_arr_real.get(); }
	constexpr T* const end() const { return m_arr_real.get() + m_Size_x * m_Size_y; }
	void swap(CLineBuffer<T>& _swap) {
		size_t x = m_Size_x, y = m_Size_y, r_x = m_RealSize_x;
		m_Size_x = _swap.m_Size_x; m_Size_y = _swap.m_Size_y; m_RealSize_x = _swap.m_RealSize_x;
		_swap.m_Size_x = x; _swap.m_Size_y = y; _swap.m_RealSize_x = r_x;

		std::swap(m_arr_real, _swap.m_arr_real);
	}
};

//要素番号探索式((y*m_Size_x)+x)