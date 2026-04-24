#ifndef _GOA_MATRIX_TEMPLATE_H_
#define _GOA_MATRIX_TEMPLATE_H_

#include <math.h>

#include "random.h"
#include "vector_template.h"
#include "quaternion_template.h"

#define matrix matrix_t<number>
#define vector vector_t<number>
#define quaternion quaternion_t<number>
#define templ template <class number>

template <class number> class matrix_t
{
	public:
	matrix_t();
	matrix_t(matrix const &other);
	matrix_t(	number a00,number a01,number a02,number a03,
				number a10,number a11,number a12,number a13,
				number a20,number a21,number a22,number a23,
				number a30,number a31,number a32,number a33);
	matrix_t(vector x,vector y,vector z,vector r);

	void set_identity();

	matrix const &operator=(const matrix &other);

	matrix const operator+(matrix const &other) const;
	matrix const operator-(matrix const &other) const;
	matrix const operator*(matrix const &other) const;
	matrix const operator*(number const other) const;
	matrix const operator/(number const other) const;

	matrix const &operator+=(matrix const &other);
	matrix const &operator-=(matrix const &other);
	matrix const &operator*=(matrix const &other);
	matrix const &operator*=(number const other);
	matrix const &operator/=(number const other);

	bool const operator==(matrix const &other) const;

	matrix const inverse() const;
	matrix const &invert();
	matrix const transposed() const;
	matrix const &transpose();

	// these only work on (or make sense for) affine 3d transform matrices
	vector const operator*(vector const &other) const;
	matrix const fast_inverse() const;
	matrix const fast_mul(matrix const &other) const;
	vector const dir_mul(vector const &other) const;

	static matrix const translate(number x,number y,number z);
	static matrix const translate(const vector &r);
	static matrix const scale(number x,number y,number z);
	static matrix const rotate_x(number angle);
	static matrix const rotate_y(number angle);
	static matrix const rotate_z(number angle);
	static matrix const rotate(number roll,number pitch,number yaw);
	static matrix const rotate(number angle,const vector &axis);
//	static matrix const skew(number x,number y,number z);
	static matrix const mirror(bool x,bool y,bool z);
	static matrix const spherical_transform(const vector &r);

	vector const x_axis() const;
	vector const y_axis() const;
	vector const z_axis() const;
	vector const position() const;

	number a[16];

	// DEPRECATED FUNCTIONS
	void make_identity();
	static matrix const translation(number x,number y,number z);
	static matrix const translation(const vector &r);
	static matrix const rotation(number x,number y,number z);
};



// constructors

templ matrix::matrix_t()
{ set_identity(); }

templ matrix::matrix_t(matrix const &other)
{ *this=other; }

templ matrix::matrix_t(	number a00,number a01,number a02,number a03,
		number a10,number a11,number a12,number a13,
		number a20,number a21,number a22,number a23,
		number a30,number a31,number a32,number a33)
{
	a[0]=a00; a[4]=a01; a[8]=a02;  a[12]=a03;
	a[1]=a10; a[5]=a11; a[9]=a12;  a[13]=a13;
	a[2]=a20; a[6]=a21; a[10]=a22; a[14]=a23;
	a[3]=a30; a[7]=a31; a[11]=a32; a[15]=a33;
}

templ matrix::matrix_t(vector x,vector y,vector z,vector r)
{
	a[0]=x.x; a[4]=y.x; a[8]=z.x;  a[12]=r.x;
	a[1]=x.y; a[5]=y.y; a[9]=z.y;  a[13]=r.y;
	a[2]=x.z; a[6]=y.z; a[10]=z.z; a[14]=r.z;
	a[3]=0;   a[7]=0;   a[11]=0;   a[15]=1;
}



templ void matrix::set_identity()
{
	a[0]=a[5]=a[10]=a[15]=1;
	a[1]=a[2]=a[3]=a[4]=a[6]=a[7]=a[8]=a[9]=a[11]=a[12]=a[13]=a[14]=0;
}



// operators

templ matrix const &matrix::operator=(const matrix &other)
{ for(int i=0;i<16;i++) a[i]=other.a[i]; return(*this); }



templ matrix const matrix::operator+(matrix const &other) const
{
	matrix res;
	for(int i=0;i<16;i++) res.a[i]=a[i]+other.a[i];
	return(res);
}

templ matrix const matrix::operator-(matrix const &other) const
{
	matrix res;
	for(int i=0;i<16;i++) res.a[i]=a[i]-other.a[i];
	return(res);
}

templ matrix const matrix::operator*(matrix const &other) const // needs speedup
{
	matrix res;
	for(int i=0;i<16;i++)
	{
		int row=i&3,column=i&12;
		double val=0;
		for(int j=0;j<4;j++) val+=a[row+j*4]*other.a[column+j];
		res.a[i]=val;
	}
	return(res);
}

templ matrix const matrix::operator*(number const other) const
{
	matrix res;
	for(int i=0;i<16;i++) res.a[i]=a[i]*other;
	return(res);
}

templ matrix const matrix::operator/(number const other) const
{
	matrix res;
	for(int i=0;i<16;i++) res.a[i]=a[i]/other;
	return(res);
}



templ matrix const &matrix::operator+=(matrix const &other)
{ for(int i=0;i<16;i++) a[i]+=other.a[i]; return(*this); }

templ matrix const &matrix::operator-=(matrix const &other)
{ for(int i=0;i<16;i++) a[i]-=other.a[i]; return(*this); }

templ matrix const &matrix::operator*=(matrix const &other) // lazy
{ return(*this=*this*other); }

templ matrix const &matrix::operator*=(number const other)
{ for(int i=0;i<16;i++) a[i]*=other; return(*this); }

templ matrix const &matrix::operator/=(number const other)
{ for(int i=0;i<16;i++) a[i]/=other; return(*this); }



templ bool const matrix::operator==(matrix const &other) const
{
	bool res=true;
	for(int i=0;i<16;i++) res=res&&(a[i]==other.a[i]);
	return(res);
}



// basic operations

templ matrix const matrix::inverse() const // has div by 0 problems - needs to choose pivot?
{
	matrix tmp(*this),res(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1); // unit

	tmp.a[4]/=tmp.a[0]; tmp.a[8]/=tmp.a[0];	tmp.a[12]/=tmp.a[0];
	res.a[0]/=tmp.a[0];
	// tmp.a[0]=1

	tmp.a[5]-=tmp.a[4]*tmp.a[1];
	tmp.a[9]-=tmp.a[8]*tmp.a[1];
	tmp.a[13]-=tmp.a[12]*tmp.a[1];
	res.a[1]-=res.a[0]*tmp.a[1];
	res.a[5]-=res.a[4]*tmp.a[1];
	res.a[9]-=res.a[8]*tmp.a[1];
	res.a[13]-=res.a[12]*tmp.a[1];
	// tmp.a[1]=0

	tmp.a[9]/=tmp.a[5]; tmp.a[13]/=tmp.a[5];
	res.a[1]/=tmp.a[5]; res.a[5]/=tmp.a[5]; res.a[9]/=tmp.a[5]; res.a[13]/=tmp.a[5];
	// tmp.a[5]=1

	tmp.a[6]-=tmp.a[4]*tmp.a[2];
	tmp.a[10]-=tmp.a[8]*tmp.a[2]+tmp.a[9]*tmp.a[6];
	tmp.a[14]-=tmp.a[12]*tmp.a[2]+tmp.a[13]*tmp.a[6];
	res.a[2]-=res.a[0]*tmp.a[2]+res.a[1]*tmp.a[6];
	res.a[6]-=res.a[4]*tmp.a[2]+res.a[5]*tmp.a[6];
	res.a[10]-=res.a[8]*tmp.a[2]+res.a[9]*tmp.a[6];
	res.a[14]-=res.a[12]*tmp.a[2]+res.a[13]*tmp.a[6];
	// tmp.a[2]=0, tmp.a[6]=0

	tmp.a[14]/=tmp.a[10];
	res.a[2]/=tmp.a[10]; res.a[6]/=tmp.a[10]; res.a[10]/=tmp.a[10]; res.a[14]/=tmp.a[10];
	// tmp.a[10]=1;

	tmp.a[7]-=tmp.a[4]*tmp.a[3];
	tmp.a[11]-=tmp.a[8]*tmp.a[3]+tmp.a[9]*tmp.a[7];
	tmp.a[15]-=tmp.a[12]*tmp.a[3]+tmp.a[13]*tmp.a[7]+tmp.a[14]*tmp.a[11];
	res.a[3]-=res.a[0]*tmp.a[3]+res.a[1]*tmp.a[7]+res.a[2]*tmp.a[11];
	res.a[7]-=res.a[4]*tmp.a[3]+res.a[5]*tmp.a[7]+res.a[6]*tmp.a[11];
	res.a[11]-=res.a[8]*tmp.a[3]+res.a[9]*tmp.a[7]+res.a[10]*tmp.a[11];
	res.a[15]-=res.a[12]*tmp.a[3]+res.a[13]*tmp.a[7]+res.a[14]*tmp.a[11];
	// tmp.a[3]=0, tmp.a[7]=0, tmp.a[11]=0

	res.a[3]/=tmp.a[15]; res.a[7]/=tmp.a[15]; res.a[11]/=tmp.a[15]; res.a[15]/=tmp.a[15];
	// tmp.a[15]=1;

	res.a[2]-=res.a[3]*tmp.a[14];
	res.a[6]-=res.a[7]*tmp.a[14];
	res.a[10]-=res.a[11]*tmp.a[14];
	res.a[14]-=res.a[15]*tmp.a[14];
	// tmp.a[14]=0

	res.a[1]-=res.a[3]*tmp.a[13]+res.a[2]*tmp.a[9];
	res.a[5]-=res.a[7]*tmp.a[13]+res.a[6]*tmp.a[9];
	res.a[9]-=res.a[11]*tmp.a[13]+res.a[10]*tmp.a[9];
	res.a[13]-=res.a[15]*tmp.a[13]+res.a[14]*tmp.a[9];
	// tmp.a[13]=0, tmp.a[9]=0

	res.a[0]-=res.a[3]*tmp.a[12]+res.a[2]*tmp.a[8]+res.a[1]*tmp.a[4];
	res.a[4]-=res.a[7]*tmp.a[12]+res.a[6]*tmp.a[8]+res.a[5]*tmp.a[4];
	res.a[8]-=res.a[11]*tmp.a[12]+res.a[10]*tmp.a[8]+res.a[9]*tmp.a[4];
	res.a[12]-=res.a[15]*tmp.a[12]+res.a[14]*tmp.a[8]+res.a[12]*tmp.a[4];
	// tmp.a[12]=0, tmp.a[8]=0, tmp.a[4]=0

	return(res);
}

templ matrix const &matrix::invert() // lazy
{ return(*this=inverse()); }

templ matrix const matrix::transposed() const
{
	return(matrix(
		a[0],a[1],a[2],a[3],
		a[4],a[5],a[6],a[7],
		a[8],a[9],a[10],a[11],
		a[12],a[13],a[14],a[15]
	));
}

templ matrix const &matrix::transpose() // lazy + unneccesary, really
{ return(*this=transposed()); }



// affine transform functions

// ignores w since it'll always be 1 for normal matrices and
// transposed matrices are only used for normals, where length
// is irrelevant
templ vector const matrix::operator*(vector const &other) const
{
	return(vector(
//		other.x*a[0]+other.y*a[1]+other.z*a[2]+a[3],
//		other.x*a[4]+other.y*a[5]+other.z*a[6]+a[7],
//		other.x*a[8]+other.y*a[9]+other.z*a[10]+a[11]
		other.x*a[0]+other.y*a[4]+other.z*a[8]+a[12],
		other.x*a[1]+other.y*a[5]+other.z*a[9]+a[13],
		other.x*a[2]+other.y*a[6]+other.z*a[10]+a[14]
	));
}

templ matrix const matrix::fast_inverse() const
{
	matrix res;

	number det;

	det=a[0]*a[5]*a[10]-a[0]*a[6]*a[9]
	+a[1]*a[6]*a[8]-a[1]*a[4]*a[10]
	+a[2]*a[4]*a[9]-a[2]*a[5]*a[8];

	// singular if det=0

	det=1/det;

	res.a[0]=(a[5]*a[10]-a[6]*a[9])*det;
	res.a[4]=-(a[4]*a[10]-a[6]*a[8])*det;
	res.a[8]=(a[4]*a[9]-a[5]*a[8])*det;

	res.a[1]=-(a[1]*a[10]-a[2]*a[9])*det;
	res.a[5]=(a[0]*a[10]-a[2]*a[8])*det;
	res.a[9]=-(a[0]*a[9]-a[1]*a[8])*det;

	res.a[2]=(a[1]*a[6]-a[2]*a[5])*det;
	res.a[6]=-(a[0]*a[6]-a[2]*a[4])*det;
	res.a[10]=(a[0]*a[5]-a[1]*a[4])*det;

	res.a[12]=-(a[12]*res.a[0]+a[13]*res.a[4]+a[14]*res.a[8]);
	res.a[13]=-(a[12]*res.a[1]+a[13]*res.a[5]+a[14]*res.a[9]);
	res.a[14]=-(a[12]*res.a[2]+a[13]*res.a[6]+a[14]*res.a[10]);

	res.a[3]=res.a[7]=res.a[11]=0;
	res.a[15]=1;

	return(res);
}

templ matrix const matrix::fast_mul(matrix const &other) const
{
	return(matrix(
	a[0]*other.a[0]+a[4]*other.a[1]+a[8]*other.a[2],
	a[0]*other.a[4]+a[4]*other.a[5]+a[8]*other.a[6],
	a[0]*other.a[8]+a[4]*other.a[9]+a[8]*other.a[10],
	a[0]*other.a[12]+a[4]*other.a[13]+a[8]*other.a[14]+a[12],

	a[1]*other.a[0]+a[5]*other.a[1]+a[9]*other.a[2],
	a[1]*other.a[4]+a[5]*other.a[5]+a[9]*other.a[6],
	a[1]*other.a[8]+a[5]*other.a[9]+a[9]*other.a[10],
	a[1]*other.a[12]+a[5]*other.a[13]+a[9]*other.a[14]+a[13],

	a[2]*other.a[0]+a[6]*other.a[1]+a[10]*other.a[2],
	a[2]*other.a[4]+a[6]*other.a[5]+a[10]*other.a[6],
	a[2]*other.a[8]+a[6]*other.a[9]+a[10]*other.a[10],
	a[2]*other.a[12]+a[6]*other.a[13]+a[10]*other.a[14]+a[14],

	0,0,0,1
	));
}

templ vector const matrix::dir_mul(vector const &other) const
{
	return(vector(
	a[0]*other.x+a[4]*other.y+a[8]*other.z,
	a[1]*other.x+a[5]*other.y+a[9]*other.z,
	a[2]*other.x+a[6]*other.y+a[10]*other.z
	));
}



// static constructors

templ matrix const matrix::translate(number x,number y,number z)
{ return(matrix(1,0,0,x,0,1,0,y,0,0,1,z,0,0,0,1)); }

templ matrix const matrix::translate(const vector &r)
{ return(matrix(1,0,0,r.x,0,1,0,r.y,0,0,1,r.z,0,0,0,1)); }

templ matrix const matrix::scale(number x,number y,number z)
{ return(matrix(x,0,0,0,0,y,0,0,0,0,z,0,0,0,0,1)); }

templ matrix const matrix::rotate_x(number angle)
{
	return(matrix(
		1,         0,          0,0,
		0,cos(angle),-sin(angle),0,
		0,sin(angle), cos(angle),0,
		0,         0,          0,1
	));
}

templ matrix const matrix::rotate_y(number angle)
{
	return(matrix(
		 cos(angle),0,sin(angle),0,
		          0,1,         0,0,
		-sin(angle),0,cos(angle),0,
		          0,0,         0,1
	));
}

templ matrix const matrix::rotate_z(number angle)
{
	return(matrix(
		cos(angle),-sin(angle),0,0,
		sin(angle), cos(angle),0,0,
		         0,           0,1,0,
		         0,           0,0,1
	));
}

templ matrix const matrix::rotate(number roll,number pitch,number yaw)
{
	return(rotate_y(yaw)*rotate_x(pitch)*rotate_z(roll));
}

templ matrix const matrix::rotate(number angle,const vector &axis)
{
	return(quaternion::rotation(angle,axis).make_matrix());
}

templ matrix const matrix::mirror(bool x,bool y,bool z)
{ return(matrix(x?-1:1,0,0,0,0,y?-1:1,0,0,0,0,z?-1:1,0,0,0,0,1)); }

templ matrix const matrix::spherical_transform(const vector &r)
{
	vector out=r.normalized();
	vector up=vector(0,1,0).plane_projected(out).normalized();
	vector side=out*up.normalized();
	return(matrix(side,out,up,r));
}



// data access

templ vector const matrix::x_axis() const
{ return(vector(a[0],a[1],a[2])); }
templ vector const matrix::y_axis() const
{ return(vector(a[4],a[5],a[6])); }
templ vector const matrix::z_axis() const
{ return(vector(a[8],a[9],a[10])); }
templ vector const matrix::position() const
{ return(vector(a[12],a[13],a[14])); }



// DEPRECATED FUNCTIONS

templ void matrix::make_identity()
{ set_identity(); }

templ matrix const matrix::translation(number x,number y,number z)
{ return(matrix(1,0,0,x,0,1,0,y,0,0,1,z,0,0,0,1)); }

templ matrix const matrix::translation(const vector &r)
{ return(matrix(1,0,0,r.x,0,1,0,r.y,0,0,1,r.z,0,0,0,1)); }

templ matrix const matrix::rotation(number x,number y,number z)
{
	return(rotate_z(z)*rotate_y(y)*rotate_x(x));
}



#undef vector
#undef matrix
#undef quaternion
#undef templ

#endif

