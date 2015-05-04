#ifndef VEC_2D_H
#define VEC_2D_H

template<typename T = double>
struct Vec2D {
	T x,y;

	Vec2D()
		:x(T()), y(T())
	{
	}

	Vec2D(T _x, T _y)
		:x(_x), y(_y)
	{
	}

	template<typename AnotherT>
	Vec2D(const Vec2D<AnotherT>& a)
	{
		x = a.x;
		y = a.y;
	}

	bool wrap(T maxx, T maxy) // Return true if need wrap x to [0, maxx) (similar to y)
	{
		bool ret = false;
		while (x > maxx) {
			x -= maxx;
			ret = true;
		}
		while (y > maxy) {
			y -= maxy;
			ret = true;
		}
		return ret;
	}

	void clamp_min(const Vec2D<T>& another)
	{
		x = std::max(x, another.x);
		y = std::max(y, another.y);
	}

	void clamp_max(const Vec2D<T>& another)
	{
		x = std::min(x, another.x);
		y = std::min(y, another.y);
	}

	Vec2D operator += (Vec2D v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	template<typename Scalar>
	Vec2D operator /= (Scalar d)
	{
		x /= d;
		y /= d;
		return *this;
	}
};

template<typename T>
inline bool operator < (const Vec2D<T>& lhs, const Vec2D<T>& rhs)
{
	return lhs.x < rhs.x || lhs.y < rhs.y;
}

template<typename T>
inline bool operator > (const Vec2D<T>& lhs, const Vec2D<T>& rhs)
{
	return lhs.x > rhs.x || lhs.y > rhs.y;
}

template<typename T>
inline bool operator != (const Vec2D<T>& lhs, const Vec2D<T>& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

template<typename T, typename Scalar>
Vec2D<T> operator / (const Vec2D<T>& lhs, Scalar d)
{
	return Vec2D<T>(lhs.x/d, lhs.y/d);
}

template<typename T>
Vec2D<T> operator - (const Vec2D<T>& lhs, const Vec2D<T>& rhs)
{
	return Vec2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<typename T>
Vec2D<T> operator + (const Vec2D<T>& lhs, const Vec2D<T>& rhs)
{
	return Vec2D<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}
#endif
