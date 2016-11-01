#ifndef geometry_h
#define geometry_h

#include <math.h>
#include <vector>

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


static double PI = 3.1415926535897932384626433832795;

struct HeightMap
{
	float** heights;
	int w, h;
} ;

struct Vertex
{
	Vector3 v;
	Vector3 n;
	Vertex(float x, float y, float z, float nx, float ny, float nz) : v(x,y,z), n(nx,ny,nz) {}
};

void createSphereDome(std::vector<Vertex*>* list, double radius, int slices, int stacks);
void createSphereDome(std::vector<Vertex*>* list, double radius, int slices, int stacks, int anglearea);

//Finds the height from a uv heightmap
inline float findHeight(const HeightMap &heightmap, float x, float y, float z, float offset)
{
	int h=heightmap.h-1;
	int w=heightmap.w-1;
	// Calculate u v
	Vector3 d = Vector3(x,y,z);
	Vector3::Normalize(d);

	float u = 0.5 + (atan2(d.getZ(), d.getX()) / (2*PI));
	float v = 0.5 - (2.0 * (asin(d.getY()) / (2*PI)));	
	// Calculate array position on image;

	if (isnan(u) || isnan(v)) return 0;    // some uv are comming back nan this needs to be fixed, such as removing these vertices 
	int ix = ((int)round(u*w +offset))%heightmap.w;
	int iy = (int)round(v*h);
	return heightmap.heights[iy][ix];
}

inline float getHeightFromColor(unsigned short r, unsigned short g, unsigned short b, float max)
{
	return (max*r*2.0f/255.0f) - max;
}

#endif /* geometry_h */
