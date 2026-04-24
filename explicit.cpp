#include "explicit.h"



#define DIFF_EPSILON 0.00000001 // 10 nano
#define COMP_EPSILON 0.00000001 // 10 nano
#define SEARCH_EPSILON 0.00000001 // 10 nano

#define aresame(a,b) ((a-b).lengthsq()<COMP_EPSILON*COMP_EPSILON)



xplicit::xplicit():playfield()
{
	u_count_mult=u_count_div=1;

	u_connected=v_connected=false;
}

void xplicit::prepare_dots(int detailmult)
{
	precalc();

	playfield::prepare_dots(detailmult);
}

void xplicit::prepare_lines(int detailmult)
{
	precalc();

//	bool nostart,noend;
	int u_steps=(detailmult*4*u_count_mult)/u_count_div;
	int v_steps=(detailmult*4*u_count_div)/u_count_mult;

//	nostart=aresame(r_func(0),0);
//	noend=aresame(r_func(1),0)||connected;

	int u_steps_corr=u_steps-(u_connected?0:1);
	int v_steps_corr=v_steps-(v_connected?0:1);

	alloc_arrays(u_steps*v_steps,u_steps_corr*v_steps+u_steps*v_steps_corr);

	int n;
	double u,v;

	n=0;
	for(int i=0;i<v_steps;i++)
	{
		v=double(i)/double(v_steps_corr);

		for(int j=0;j<u_steps;j++)
		{
			u=double(j)/double(u_steps_corr);
			if(onesided) u/=2;

			vector pos=func(u,v),normal;
			make_normal(u,v,normal);
			vertices[n].pos=vector_f(pos.x,pos.y,pos.z);
			vertices[n].n=vector_f(normal.x,normal.y,normal.z);

			n++;
		}
	}

	n=0;

	for(int i=0;i<v_steps;i++)
	for(int j=0;j<u_steps_corr;j++)
	{
		int ii=i;
		int jj=j+1;
		if(j==u_steps_corr-1)
		if(u_connected)
		{
			jj=0;
			if(onesided==ONESIDED_MOEBIUS) ii=v_steps-1-i;
			else if(onesided==ONESIDED_KLEIN) ii=(v_steps+v_steps/2-i)%v_steps_corr;
		}

		lines[n].a=i*u_steps+j;
		lines[n].b=ii*u_steps+jj;
		n++;
	}

	for(int i=0;i<v_steps_corr;i++)
	for(int j=0;j<u_steps;j++)
	{
		int ii=i+1;
		if(i==v_steps_corr-1)
		if(v_connected) ii=0;

		lines[n].a=i*u_steps+j;
		lines[n].b=ii*u_steps+j;
		n++;
	}
}

void xplicit::precalc()
{
/*	double t,r,z,z_o,r_o;

	z_o=z_func(0);
	r_o=r_func(0);
	area=0;
	for(int i=1;i<=INT_STEPS;i++)
	{
		t=INT_STEPSIZE*double(i);
		z=z_func(t);
		r=r_func(t);

		area+=sqrt((z-z_o)*(z-z_o)+(r-r_o)*(r-r_o))*(r+r_o)/2; //*PI
		z_o=z;
		r_o=r;
	}*/

	u_connected=aresame(_func(0,0),_func(1,0))&&aresame(_func(0,1),_func(1,1));
	v_connected=aresame(_func(0,0),_func(0,1))&&aresame(_func(1,0),_func(1,1));

//	u_connected=true;
}

void xplicit::recalc(vector &pos,vector &dir,vector &normal,vector &prev)
{
	double u,v;
	vector r;

	u=prev.x;
	v=prev.y;

	double u_coord,v_coord;

	do
	{
		vector u_base=u_diff(u,v);
		vector v_base=v_diff(u,v);

		double u_len=u_base.length();
		u_base/=u_len; // normalize

		double corr=v_base.dot(u_base);
		v_base-=u_base*corr; // orthogonalize

//		if(v_base.dot(u_base)>0.0000001) die("AAAH!");

		double v_len=v_base.length();
		v_base/=v_len; // normalize

		vector r=func(u,v);

		u_coord=u_base.dot(pos-r);
		v_coord=v_base.dot(pos-r);

		v_coord/=v_len;
		u_coord=(u_coord-v_coord*corr)/u_len; // transform back

		double d2=dist2(u,v,pos);

		while(dist2(u+u_coord,v+v_coord,pos)>d2)
		{
			u_coord/=2;
			v_coord/=2;
		}

		u+=u_coord;
		v+=v_coord;

		if(u<0&&!u_connected) { u=0; u_coord=0; }
		if(u>1&&!u_connected) { u=1; u_coord=0; }
		if(v<0&&!v_connected) { v=0; v_coord=0; }
		if(v>1&&!v_connected) { v=1; v_coord=0; }
	}
	while(
		fabs(u_coord)>SEARCH_EPSILON
		&&fabs(v_coord)>SEARCH_EPSILON
	);

	pos=func(u,v);
	make_normal(u,v,normal);

	prev.x=u;
	prev.y=v;

/*	double u_left,u_right,v_left,v_right;

	double stepsize=u_stepsize;

	while(stepsize>SEARCH_EPSILON)
	{
		double u_step=-dist2_u_diff(u,v,pos);
		double v_step=-dist2_v_diff(u,v,pos);
		double fac=stepsize/sqrt(u_step*u_step+v_step*v_step);
		u_step*=fac;
		v_step*=fac;

		double dot=u_step*dist2_u_diff(u+u_step,v+v_step,pos)+v_step*dist2_v_diff(u+u_step,v+v_step,pos);

		if(dot>0) // too far - shrink
		{
			while(
				//((!u_connected&&((u+u_step)>1||(u+u_step)<0))||(!v_connected&&((v+v_step)>1||(v+v_step)<0)))
				//||
				dot>0
			)
			{
				u_step/=2;
				v_step/=2;
				stepsize/=2;
				if(stepsize<SEARCH_EPSILON) break;
				dot=u_step*dist2_u_diff(u+u_step,v+v_step,pos)+v_step*dist2_v_diff(u+u_step,v+v_step,pos);
			}

			u_left=u+u_step; // negative dot product
			v_left=v+v_step;
			u_right=u+u_step*2; // positive dot product
			v_right=v+v_step*2;
		}
		else // not far enough - extend
		{
			while(
				//!((!u_connected&&((u+u_step)>1||(u+u_step)<0))||(!v_connected&&((v+v_step)>1||(v+v_step)<0)))
				//&&
				dot<0
			)
			{
				u_step*=2;
				v_step*=2;
				stepsize*=2;
				dot=u_step*dist2_u_diff(u+u_step,v+v_step,pos)+v_step*dist2_v_diff(u+u_step,v+v_step,pos);
			}

			u_left=u+u_step/2; // negative dot product
			v_left=v+v_step/2;
			u_right=u+u_step; // positive dot product
			v_right=v+v_step;
		}

		while(
			fabs(u_left-u_right)>SEARCH_EPSILON&&
			fabs(v_left-v_right)>SEARCH_EPSILON
		)
		{
			double u_mid=(u_left+u_right)/2;
			double v_mid=(v_left+v_right)/2;
			dot=u_step*dist2_u_diff(u_mid,v_mid,pos)+v_step*dist2_v_diff(u_mid,v_mid,pos);

			if(dot<0) { u_left=u_mid; v_left=v_mid; }
			else { u_right=u_mid; v_right=v_mid; }
		}


		u=u_left;
		v=v_left;

		//u+=u_step;
		//v+=v_step;
	}

	pos=func(u,v);
	make_normal(u,v,normal);

	prev.x=u;
	prev.y=v;*/

	dir.plane_project(normal);
	dir.normalize(); // project heading onto plane + normalize

	if(!u_connected)
	if(u==0||u==1)
	{
		vector edge=v_diff(u,v).normalized();

		dir=-dir+edge*2*dir.dot(edge);
	}

	if(!v_connected)
	if(v==0||v==1)
	{
		vector edge=u_diff(u,v).normalized();

		dir=-dir+edge*2*dir.dot(edge);
	}
}



void xplicit::random_pos(vector &pos,vector &normal,vector &prev)
{
/*	double t,z1,r1,z2,r2,p;

	do
	{
		t=drand();

		z1=z_func(t-RAND_EPSILON);
		r1=r_func(t-RAND_EPSILON);
		z2=z_func(t+RAND_EPSILON);
		r2=r_func(t+RAND_EPSILON);

		// generate probability for Monte Carlo routine
		p=sqrt((z1-z2)*(z1-z2)+(r1-r2)*(r1-r2))*(r1+r2)/2/(2*RAND_EPSILON)/area; //*PI
	} while(drand()>p);

	double z,r,a,sin_a,cos_a,r_n;

	z=z_func(t);
	r=r_func(t);
	a=drand()*2*PI;
	sin_a=sin(a);
	cos_a=cos(a);

	pos.x=cos_a*r;
	pos.y=sin_a*r;
	pos.z=z;

	make_normal(t,cos_a,sin_a,normal);

	prev.x=t;
*/

	double u=drand(),v=drand();

	pos=func(u,v);
	make_normal(u,v,normal);
	prev.x=u;
	prev.y=v;
}

vector xplicit::_func(double u,double v) { return(vector(0,0,0)); }

vector xplicit::func(double u,double v)
{
	if(u_connected) u-=floor(u);
	if(v_connected) v-=floor(v);
	return(_func(u,v));
}

vector xplicit::u_diff(double u,double v)
{
	double upper=u+DIFF_EPSILON;
	double lower=u-DIFF_EPSILON;

	if(!u_connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((func(upper,v)-func(lower,v))/(upper-lower));
}

vector xplicit::v_diff(double u,double v)
{
	double upper=v+DIFF_EPSILON;
	double lower=v-DIFF_EPSILON;

	if(!v_connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((func(u,upper)-func(u,lower))/(upper-lower));
}

double xplicit::dist2(double u,double v,const vector &r)
{
	return((r-func(u,v)).lengthsq());
}

double xplicit::dist2_u_diff(double u,double v,const vector &r)
{
	double upper=u+DIFF_EPSILON;
	double lower=u-DIFF_EPSILON;

	if(!u_connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((dist2(upper,v,r)-dist2(lower,v,r))/(upper-lower));
}

double xplicit::dist2_v_diff(double u,double v,const vector &r)
{
	double upper=v+DIFF_EPSILON;
	double lower=v-DIFF_EPSILON;

	if(!v_connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((dist2(u,upper,r)-dist2(u,lower,r))/(upper-lower));
}

void xplicit::make_normal(double u,double v,vector &n)
{
//	n=func(u,v).normalized();

	n=(u_diff(u,v)*v_diff(u,v)).normalized();
}




moebius::moebius():xplicit() { name="Möbius strip"; ident="moebius"; onesided=ONESIDED_MOEBIUS; u_count_mult=2; }

vector moebius::_func(double u,double v)
{
	u*=4*PI;
	v=v*2-1;
	return(vector(
		(1+v*cos(u/2))*cos(u),
		(1+v*cos(u/2))*sin(u),
		v*sin(u/2)
	)/2);
}

mtwist::mtwist():xplicit() { name="Möbius twist"; ident="mtwist"; onesided=ONESIDED_MOEBIUS; u_count_mult=2; }

vector mtwist::_func(double u,double v)
{
	u*=4*PI;
	v=v*2-1;
	return(vector(
		(1.05+v*cos(3*u/2))*cos(u),
		(1.05+v*cos(3*u/2))*sin(u),
		v*sin(3*u/2)
	)/2);
}

klein::klein():xplicit() { name="Klein bottle"; ident="klein"; onesided=ONESIDED_KLEIN; u_count_mult=2; u_count_div=1; }

vector klein::_func(double u,double v)
{
	u*=4*PI;
	v*=2*PI;
	u+=PI/4;

//	double a=0.5; // Slider a 0 1 .5
//	double c=0.15; // Slider c 0 1 .15
	double a=0.5; // Slider a 0 1 .5
	double c=0.15; // Slider c 0 1 .15
	double sc=0.7;

	vector dir=vector(0,1,0);

	if(u<5*PI/4)
	{
		vector pos=vector(a*(sin(2*u)-1)/2,0,-cos(u));
		vector deriv=vector(a*cos(2*u),0,sin(u));
		vector cross=dir*deriv.normalized();
		return ((pos+cross*c*cos(PI-v)+dir*c*sin(PI-v))*sc)*1.3;
	}
	else if(u<9*PI/4)
	{
		double r=c+a*(1+sin(2*u-3*PI))/2;
		return(vector(r*cos(v),r*sin(v),-cos(u))*sc)*1.3;
	}
	else if(u<13*PI/4)
	{
		vector pos=vector(a*(sin(2*u)-1)/2,0,-cos(u));
		vector deriv=vector(a*cos(2*u),0,sin(u));
		vector cross=dir*deriv.normalized();
		return((pos+cross*c*cos(v)+dir*c*sin(v))*sc)*1.3;
	}
	else
	{
		double r=c+a*(1+sin(2*u-3*PI))/2;
		return(vector(r*cos(PI-v),r*sin(PI-v),-cos(u))*sc)*1.3;
	}


/*	double r=4*(1-cos(u)/2);

	if(u<PI)
	return(vector(
		(6*cos(u)*(1+sin(u))+r*cos(u)*cos(v)),
		(16*sin(u)+r*sin(u)*cos(v)),
		r*sin(v)
	)/16);
	else if(u<2*PI)
	return(vector(
		(6*cos(u)*(1+sin(u))-r*cos(v)),
		(16*sin(u)),
		r*sin(v)
	)/16);
	else if(u<3*PI)
	return(vector(
		(6*cos(u)*(1+sin(u))-r*cos(u)*cos(v)),
		(16*sin(u)-r*sin(u)*cos(v)),
		r*sin(v)
	)/16);
	else
	return(vector(
		(6*cos(u)*(1+sin(u))+r*cos(v)),
		(16*sin(u)),
		r*sin(v)
	)/16);*/
}

trefoil::trefoil():xplicit() { name="Trefoil knot"; ident="trefoil"; u_count_mult=3; }

vector trefoil::_func(double u,double v)
{
	u*=4*PI;
	v*=2*PI;

	double r_min=0.2,r_min_z=0.33,r_maj=0.66,r_knot=0.2;

	double r=r_maj+r_min*cos(1.5*u);

	vector pos=vector(
		r*cos(u),
		r*sin(u),
		r_min_z*sin(1.5*u)
	);

	vector deriv=vector(
		-1.5*r_min*sin(1.5*u)*cos(u)-r*sin(u),
		-1.5*r_min*sin(1.5*u)*sin(u)+r*cos(u),
		1.5*r_min*cos(1.5*u)
	).normalized();

	vector out=(pos-deriv*pos.dot(deriv)).normalized();

	vector cross=(out*deriv).normalized();

/*	vector deriv2=vector(
		-1.5*1.5*r_min*cos(1.5*u)*cos(u)+1.5*r_min*sin(1.5*u)*sin(u)+1.5*r_min*sin(1.5*u)*sin(u)+r*cos(u),
		-1.5*1.5*r_min*cos(1.5*u)*sin(u)-1.5*r_min*sin(1.5*u)*cos(u)-1.5*r_min*sin(1.5*u)*cos(u)-r*sin(u),
		-1.5*1.5*r_min*sin(1.5*u)
	).normalized();

	vector cross=(deriv2*deriv).normalized();*/

	return(pos+(out*cos(v)+cross*sin(v))*r_knot);
}




// http://www.uib.no/People/nfytn/surfaces.htm

etest::etest():xplicit() { name="Explicit test"; ident=0; onesided=0; u_count_mult=4; }

vector etest::_func(double u,double v)
{
	// plus-torus

	u*=8*PI;
	v=v*2-1;
	return(vector(
		(1+v*cos(u/4))*cos(u),
		(1+v*cos(u/4))*sin(u),
		v*sin(u/4)
	)/2);

	// square torus twist

	/*u*=8*PI;
	v=v*2-1;

	double r_maj=0.6;
	double halfside=0.3;

	double r=r_maj+halfside*(cos(u/4)-v*sin(u/4));
	double z=halfside*(sin(u/4)+v*cos(u/4));

	return(vector(
		r*cos(u),
		r*sin(u),
		z
	));*/
}

/*

HyperSwear
==========

[cos(u),sin(u),cos(v),sin(v)] // torus


---

The Klein bottle embeds nicely in S^2 x S^1, which embeds nicely in R^4.

Let c(s) denote (cos(s), sin(s), 0, 0). 

Then one parametric representation of K in R^4 is given by

  c(s) + r*(cos(t)*[cos(s/2)*c(s) + sin(s/2)*(0 0 1 0)] + sin(t)*(0 0 0 1)),

which works out to


  x(s,t) = cos(s)*(1 + r*cos(t)*cos(s/2))

  y(s,t) = sin(s)*(1 + r*cos(t)*cos(s/2))

  z(s,t) = r*cos(t)*sin(s/2)

  w(s,t) = r*sin(t)


  for 0 <= s < 2*pi and 0 <= t < 2*pi,

where r = any positive constant < 1, say r = 1/2.

---

(klein)

 x = (a + b*sin(v))*cos(u);
 y = (a + b*sin(v))*sin(u);
 z = b*cos(v)*cos(u*0.5);
 w = b*cos(v)*sin(u*0.5);

---

Lets try the projective sphere.   I'm not saying that what follows is
clear or rigorous, but you may be able to follow my thinking!

    0 <= theta <= pi/2
    0 <= phi <= 2 pi

    f( theta, 2 pi ) = f( theta, 0 )
    f( 0, phi_1 ) = f( 0, phi_2 )
    f( pi/2, phi ) = f( pi/2, pi + phi )

Eligible functions include:

    cos( 2 phi )
    sin( 2 phi )
    sin( 2 theta ) cos( phi )
    sin( 2 theta ) sin( phi )
    cos( 2 theta )

(Chemists will recognise a connection with the d-orbitals!)

You want a four-dimensional embedding, so why not try:
    x1 = cos( 2 phi )
    x2 = sin( 2 theta ) cos( phi )
    x3 = sin( 2 theta ) sin( phi )
    x4 = cos( 2 theta )



For a torus:
    x1 = cos( theta ) cos( phi )
    x2 = cos( theta ) sin( phi )
    x3 = sin( theta ) cos( phi )
    x4 = sin( theta ) sin( phi )
    0 <= theta < 2 pi
    0 <= phi < 2 pi

(I am aware that there is an embedding into R^3, but this is
the "orbital" solution.   Also, this gives an embedding into S^3 and
there is no embedding into S^2)

For a Klein bottle:
    x1 = cos( theta ) sin( phi )
    x2 = sin( theta ) sin( phi )
    x3 = cos( 2 theta ) cos( phi )
    x4 = sin( 2 theta ) cos( phi )
    0 <= theta < pi
    0 <= phi < 2 pi

---

>There is a parametric equation for Boy's surface in this page:
>
>http://www.uib.no/People/nfytn/boytxt.htm
>
>I'm trying to draw it but I keep getting odd results. Is there something
>wrong with the equation? If not, what are the bounds for u and v?
>
>The equation is
>
>x = (2/3)*(cos(u)*cos(2*v)+sqrt(2)*sin(u)*cos(v))*cos(u) /
>        (sqrt(2) - sin(2*u)*sin(3*v))
>
>y = (2/3)*(cos(u)*sin(2*v)-sqrt(2)*sin(u)*sin(v))*cos(u) /
>        (sqrt(2) - sin(2*u)*sin(3*v))
>
>z = sqrt(2)*cos(u)^2 / (sqrt(2) - sin(2*u)*sin(2*v))
>
>
>Joni
>
>
The bounds for u and v are
0 <= u <= pi
0 <= v <= pi

According to some testing, it appears to be that the equation is, in fact,
wrong. The denominator of the z coorinate should be (sqrt(2) -
sin(2*u)*sin(3*v)), like that of x and y.

*/
