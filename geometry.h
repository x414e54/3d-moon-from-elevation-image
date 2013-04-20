#include <math.h>

class Vector3
{
private:
	float _1;
	float _2;
	float _3;
public:
	Vector3(float _1=0.0f, float _2=0.0f, float _3=0.0f) : _1(_1), _2(_2), _3(_3) {}

	Vector3 operator*(const Vector3&v) const
	{
		return Vector3(_1*v._1, _2*v._2, _3*v._3);
	}

	float getX() { return _1; }
	float getY() { return _2; }
	float getZ() { return _3; }

	void operator*=(const Vector3&v)
	{
		_1*=v._1;
		_2*=v._2;
		_3*=v._3;
	}

	Vector3 operator*(const float scalar) const
	{
		return Vector3(_1*scalar, _2*scalar, _3*scalar);
	}

	void operator*=(const float scalar)
	{
		_1*=scalar;
		_2*=scalar;
		_3*=scalar;
	}

	void operator/=(const float scalar)
	{
		_1/=scalar;
		_2/=scalar;
		_3/=scalar;
	}

	Vector3 operator/(const float scalar) const
	{
		return Vector3(_1/scalar, _2/scalar, _3/scalar);
	}

	Vector3 operator+(const Vector3&v) const
	{
		return Vector3(_1+v._1, _2+v._2, _3+v._3);
	}

	void operator+=(const Vector3&v)
	{
		_1+=v._1;
		_2+=v._2;
		_3+=v._3;
	}

	Vector3 operator-(const Vector3&v) const
	{
		return Vector3(_1-v._1, _2-v._2, _3-v._3);
	}

	void operator-=(const Vector3&v)
	{
		_1-=v._1;
		_2-=v._2;
		_3-=v._3;
	}

	void operator=(const Vector3&v)
	{
		_1=v._1;
		_2=v._2;
		_3=v._3;
	}

	static float ScalarProduct(Vector3 &v, Vector3 &v2)
	{
		return (
			(v._1*v2._1) +
			(v._2*v2._2) +
			(v._3*v2._3)
			);
	}

	static void VectorProduct(Vector3 &v, Vector3 &v2)
	{	
		v._1=(v._2*v2._3) - (v._3*v2._2);
		v._2=(v._3*v2._1) - (v._1*v2._3);
		v._3=(v._1*v2._2) - (v._2*v2._1);
	}	

	static float Magnitude(const Vector3 &v)
	{
		return sqrt((v._1*v._1)+(v._2*v._2)+(v._3*v._3));
	}

	static void Normalize(Vector3 &v)
	{
		v=v/Magnitude(v);
	}

	static float SquareMagnitude(const Vector3 &v)
	{
		return ((v._1*v._1)+(v._2*v._2)+(v._3*v._3));
	}

};
