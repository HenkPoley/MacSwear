#ifndef _GOA_VECTOR_TEMPLATE_H_
#define _GOA_VECTOR_TEMPLATE_H_

#include <math.h>

#include "random.h"

#define vector vector_t<number>
#define templ template <class number>

template <class number> class vector_t
{
	public:
	vector_t();
	vector_t(vector const &other);
	vector_t(number x0,number y0,number z0);

	vector const &operator=(const vector &other);

	vector const operator+(vector const &other) const;
	vector const operator-(vector const &other) const;
	vector const operator-(void) const;
	vector const operator*(vector const &other) const;
	vector const operator*(number const other) const;
	vector const operator/(number const other) const;

	vector const &operator+=(vector const &other);
	vector const &operator-=(vector const &other);
	vector const &operator*=(vector const &other);
	vector const &operator*=(number const other);
	vector const &operator/=(number const other);

	bool const operator==(vector const &other) const;

	number abs() const;
	number abs2() const;
	number dot(vector const &other) const;

	vector const &normalize();
	vector const normalized() const;

	vector const &plane_project(const vector &normal); // requires UNIT normal!
	vector const plane_projected(const vector &normal) const;

	static vector rand_cube();
	static vector rand_sphere();

	number x,y,z;

	// DEPRECATED FUNCTIONS
	number length() const;
	number lengthsq() const;
};



// constructors

templ vector::vector_t() { x=y=z=0; }
templ vector::vector_t(vector const &other) { *this=other; }
templ vector::vector_t(number x0,number y0,number z0) { x=x0; y=y0; z=z0; }



// operators

templ vector const &vector::operator=(vector const &other)
{ x=other.x; y=other.y; z=other.z; return(*this); }



templ vector const vector::operator+(vector const &other) const
{ return(vector(x+other.x,y+other.y,z+other.z)); }

templ vector const vector::operator-(vector const &other) const
{ return(vector(x-other.x,y-other.y,z-other.z)); }
templ vector const vector::operator-() const
{ return(vector(-x,-y,-z)); }

templ vector const vector::operator*(vector const &other) const
{
	return(vector(
		y*other.z-z*other.y,
		z*other.x-x*other.z,
		x*other.y-y*other.x
	));
}
templ vector const vector::operator*(number const other) const
{ return(vector(x*other,y*other,z*other)); }

templ vector const vector::operator/(number const other) const
{ return(vector(x/other,y/other,z/other)); }



templ vector const &vector::operator+=(vector const &other)
{ x+=other.x; y+=other.y; z+=other.z; return(*this); }

templ vector const &vector::operator-=(vector const &other)
{ x-=other.x; y-=other.y; z-=other.z; return(*this); }

templ vector const &vector::operator*=(vector const &other)
{ return(*this=*this*other); }
templ vector const &vector::operator*=(number const other)
{ x*=other; y*=other; z*=other; return(*this); }

templ vector const &vector::operator/=(number const other)
{ x/=other; y/=other; z/=other; return(*this); }



templ bool const vector::operator==(vector const &other) const
{ return(x==other.x&&y==other.y&&z==other.z); }



// basic functions

templ number vector::abs() const
{ return(sqrt(x*x+y*y+z*z)); }

templ number vector::abs2() const
{ return(x*x+y*y+z*z); }

templ number vector::dot(vector const &other) const
{ return(x*other.x+y*other.y+z*other.z); }

templ vector const &vector::normalize()
{
	number inv=1/abs();
	x*=inv; y*=inv; z*=inv;
	return(*this);
}

templ vector const vector::normalized() const
{
	number inv=1/abs();
	return(vector(x*inv,y*inv,z*inv));
}



templ vector const &vector::plane_project(const vector &normal)
{
	*this-=normal*(normal.dot(*this)); // project vector onto tangent plane
	return(*this);
}

templ vector const vector::plane_projected(const vector &normal) const
{
	return(*this-normal*(normal.dot(*this)));
}



// static constructors

templ vector vector::rand_cube()
{
	return(vector(2*drand()-1,2*drand()-1,2*drand()-1));
}

templ vector vector::rand_sphere()
{
	vector res;

	do
	{
		res.x=2*drand()-1;
		res.y=2*drand()-1;
		res.z=2*drand()-1;
	} while(res.lengthsq()>1);

	return(res);
}



// DEPRECATED FUNCTIONS

templ number vector::length() const
{ return(abs()); }

templ number vector::lengthsq() const
{ return(abs2()); }



#undef vector
#undef templ

#endif

