#ifndef _GOA_QUATERNION_TEMPLATE_H_
#define _GOA_QUATERNION_TEMPLATE_H_

#include <math.h>

#include "random.h"
#include "vector_template.h"
#include "matrix_template.h"

#define vector vector_t<number>
#define matrix matrix_t<number>
#define quaternion quaternion_t<number>
#define templ template <class number>

template <class number> class matrix_t;

template <class number> class quaternion_t
{
	public:
	quaternion_t();
	quaternion_t(quaternion const &other);
	quaternion_t(number w0);
	quaternion_t(number w0,number x0,number y0,number z0);
	quaternion_t(number s0,vector const &v0);

	quaternion const &operator=(const quaternion &other);
	quaternion const &operator=(const number other);

	quaternion const operator+(quaternion const &other) const;
	quaternion const operator+(number const other) const;
	quaternion const operator-(quaternion const &other) const;
	quaternion const operator-(number const other) const;
	quaternion const operator-() const;
	quaternion const operator*(quaternion const &other) const;
	quaternion const operator*(number const other) const;
	quaternion const operator/(quaternion const &other) const;
	quaternion const operator/(number const other) const;

	quaternion const &operator+=(quaternion const &other);
	quaternion const &operator+=(number const other);
	quaternion const &operator-=(quaternion const &other);
	quaternion const &operator-=(number const other);
	quaternion const &operator*=(quaternion const &other);
	quaternion const &operator*=(number const other);
	quaternion const &operator/=(quaternion const &other);
	quaternion const &operator/=(number const other);

	bool const operator==(quaternion const &other) const;
	bool const operator==(number const other) const;

	number abs() const;
	number abs2() const;
	quaternion const conj() const;
	quaternion const conjugated() const;
	quaternion const &conjugate();
	quaternion const normalized() const;
	quaternion const &normalize();
	quaternion const inverse() const;
	quaternion const &invert();

	// these assume a unit quaternion!
	vector const apply_rotation(vector &p) const;
	matrix const make_matrix() const;

	static quaternion const rotation(number angle,const vector &axis);

	vector v;
	number s;

	// DEPRECATED FUNCTIONS
	number length() const;
	number lengthsq() const;
};



// constructors

templ quaternion::quaternion_t() { s=0; }
templ quaternion::quaternion_t(quaternion const &other) { *this=other; }
templ quaternion::quaternion_t(number w0) { s=w0; }
templ quaternion::quaternion_t(number w0,number x0,number y0,number z0) { s=w0; v.x=x0; v.y=y0; v.z=z0; }
templ quaternion::quaternion_t(number s0,vector const &v0) { s=s0; v=v0; }



// operators

templ quaternion const &quaternion::operator=(quaternion const &other)
{ s=other.s; v=other.v; return(*this); }
templ quaternion const &quaternion::operator=(number const other)
{ s=other; v=vector(0,0,0); return(*this); }



templ quaternion const quaternion::operator+(quaternion const &other) const
{ return(quaternion(s+other.s,v+other.v)); }
templ quaternion const quaternion::operator+(number const other) const
{ return(quaternion(s+other,v)); }

templ quaternion const quaternion::operator-(quaternion const &other) const
{ return(quaternion(s-other.s,v-other.v)); }
templ quaternion const quaternion::operator-(number const other) const
{ return(quaternion(s-other,v)); }
templ quaternion const quaternion::operator-() const
{ return(quaternion(-s,-v)); }

templ quaternion const quaternion::operator*(quaternion const &other) const
{
	return(quaternion(
		s*other.s-v.dot(other.v),
		other.v*s+v*other.s+v*other.v
	));
}
templ quaternion const quaternion::operator*(number const other) const
{ return(quaternion(s*other,v*other)); }

templ quaternion const quaternion::operator/(quaternion const &other) const
{ return(*this*other.inverse()); }
templ quaternion const quaternion::operator/(number const other) const
{ return(quaternion(s/other,v/other)); }



templ quaternion const &quaternion::operator+=(quaternion const &other)
{ s+=other.s; v+=other.v; return(*this); }
templ quaternion const &quaternion::operator+=(number const other)
{ s+=other; return(*this); }

templ quaternion const &quaternion::operator-=(quaternion const &other)
{ s-=other.s; v-=other.v; return(*this); }
templ quaternion const &quaternion::operator-=(number const other)
{ s-=other; return(*this); }

templ quaternion const &quaternion::operator*=(quaternion const &other) // lazy
{ return(*this=*this*other); }
templ quaternion const &quaternion::operator*=(number const other)
{ s*=other; v*=other; return(*this); }

templ quaternion const &quaternion::operator/=(quaternion const &other) // lazy
{ return(*this=*this/other); }
templ quaternion const &quaternion::operator/=(number const other)
{ s/=other; v/=other; return(*this); }



templ bool const quaternion::operator==(quaternion const &other) const
{ return(s==other.s&&v==other.v); }
templ bool const quaternion::operator==(number const other) const
{ return(s==other.s&&v==vector(0,0,0)); }



// basic functions

templ number quaternion::abs() const
{ return(sqrt(s*s+v.x*v.x+v.y*v.y+v.z*v.z)); }

templ number quaternion::abs2() const
{ return(s*s+v.x*v.x+v.y*v.y+v.z*v.z); }

templ quaternion const quaternion::conj() const
{ return(quaternion(s,-v)); }

templ quaternion const quaternion::conjugated() const
{ return(quaternion(s,-v)); }

templ quaternion const &quaternion::conjugate()
{ v=-v; return(*this); }

templ quaternion const quaternion::normalized() const
{
	number inv=1/abs();
	return(quaternion(s*inv,v*inv));
}

templ quaternion const &quaternion::normalize()
{
	number inv=1/abs();
	s*=inv; v*=inv;
	return(*this);
}

templ quaternion const quaternion::inverse() const
{ return(conj()/abs2()); }

templ quaternion const &quaternion::invert()
{ conjugate(); *this/=abs2(); return(*this); }



// rotation functions

templ vector const quaternion::apply_rotation(vector &p) const
{
//	return((conjugated()*quaternion(0,p)**this).v);
	return(v*2*(v.dot(p))+(v*p)*2*s+p*(2*s*s-1)); // hopefully
}

templ matrix const quaternion::make_matrix() const
{
	number s2=s*s,x2=v.x*v.x,y2=v.y*v.y,z2=v.z*v.z;
	return(matrix(
		s2+x2-y2-z2,2*(v.x*v.y-s*v.z),2*(v.x*v.z+s*v.y),0,
		2*(v.x*v.y+s*v.z),s2-x2+y2-z2,2*(v.y*v.z-s*v.x),0,
		2*(v.x*v.z-s*v.y),2*(v.y*v.z+s*v.x),s2-x2-y2+z2,0,
		0,0,0,1
	));
}



// static constructors

templ quaternion const quaternion::rotation(number angle,const vector &axis)
{ return(quaternion(cos(angle/2),axis*sin(angle/2))); }



// DEPRECATED FUCNTIONS

templ number quaternion::length() const
{ return(abs()); }

templ number quaternion::lengthsq() const
{ return(abs2()); }



#undef vector
#undef matrix
#undef quaternion
#undef templ

#endif
