#include "lathe.h"



#define INT_STEPS 128
#define INT_STEPSIZE (1.0/INT_STEPS)
// step count does NOT include end point!
#define DIFF_EPSILON 0.00000001 // 10 nano
#define COMP_EPSILON 0.00000001 // 10 nano
#define RAND_EPSILON 0.00000001 // 10 nano
#define SEARCH_EPSILON 0.00000001 // 10 nano

#define aresame(a,b) (ABS((a)-(b))<COMP_EPSILON)
#define ABS(a) ((a)<0?-(a):(a))



lathe::lathe():playfield()
{
	stepsize=1.0/128.0;
}

void lathe::prepare_dots(int detailmult)
{
	precalc();

	playfield::prepare_dots(detailmult);
}

void lathe::prepare_lines(int detailmult)
{
	precalc();

	bool nostart,noend;
	int func_steps=detailmult*4;
	int func_segs=detailmult*4;

	nostart=aresame(r_func(0),0);
	noend=aresame(r_func(1),0)||connected;

	alloc_arrays(func_steps*func_segs,
	(func_steps-(nostart?1:0)-(noend?1:0))*func_segs+(func_steps-1)*func_segs);

	int n;
	double a,t,r,z,sin_a,cos_a;

	n=0;
	for(int i=0;i<func_steps;i++)
	{
		t=double(i)/double(func_steps-1);

		for(int j=0;j<func_segs;j++)
		{
			a=2*PI*double(j)/double(func_segs);
			sin_a=sin(a);
			cos_a=cos(a);

			z=z_func(t);
			r=r_func(t);

			vertices[n].pos.x=cos_a*r;
			vertices[n].pos.y=sin_a*r;
			vertices[n].pos.z=z;

			vector normal;
			make_normal(t,cos_a,sin_a,normal);
			vertices[n].n=vector_f(normal.x,normal.y,normal.z);

			n++;
		}
	}

	n=0;
	for(int i=(nostart?1:0);i<func_steps-(noend?1:0);i++)
	for(int j=0;j<func_segs;j++)
	{
		lines[n].a=i*func_segs+j;
		lines[n].b=i*func_segs+(j+1)%func_segs;
		n++;
	}

	for(int i=0;i<func_steps-1;i++)
	for(int j=0;j<func_segs;j++)
	{
		lines[n].a=i*func_segs+j;
		lines[n].b=(i+1)*func_segs+j;
		n++;
	}

	playfield::prepare_lines(detailmult);
}

void lathe::precalc()
{
	double t,r,z,z_o,r_o;

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
	}

	connected=aresame(_z_func(0),_z_func(1))&&aresame(_r_func(0),_r_func(1));
}

void lathe::recalc(vector &pos,vector &dir,vector &normal,vector &prev)
{
	double z,r,t;
	double cos_a,sin_a;

	z=pos.z;
	r=sqrt(pos.x*pos.x+pos.y*pos.y);
	cos_a=pos.x/r;
	sin_a=pos.y/r;
	t=prev.x;

	double left=t,right=t;
	double dot;

	dot=(z-z_func(t))*z_diff(t)+(r-r_func(t))*r_diff(t);

	if(dot>0)
	{
		do
		{
			right+=stepsize;
			if(right>1&&!connected) { right=1; break; }

			dot=(z-z_func(right))*z_diff(right)+(r-r_func(right))*r_diff(right);
		}
		while(dot>0);
	}
	else
	{
		do
		{
			left-=stepsize;
			if(left<0&&!connected) { left=0; break; }

			dot=(z-z_func(left))*z_diff(left)+(r-r_func(left))*r_diff(left);
		}
		while(dot<0);
	}

	while(fabs(left-right)>SEARCH_EPSILON)
	{
		if(dist2(left,z,r)<dist2(right,z,r)) right=(left+right)/2;
		else left=(left+right)/2;
	}

	left=left-floor(left);

	r=r_func(left);
	pos.x=cos_a*r;
	pos.y=sin_a*r;
	pos.z=z_func(left);

	make_normal(left,cos_a,sin_a,normal);

	prev.x=left;

/*
	double z_p,r_p,cos_a,sin_a;
	double r_t;
	double t,t_,d2,t_min,d2_min;
	double left,right;
	double r_n;
//	static double t_old=0;
	int i;

	z_p=pos.z;
	r_p=sqrt(pos.x*pos.x+pos.y*pos.y);
	cos_a=pos.x/r_p;
	sin_a=pos.y/r_p;

	d2_min=1000; // large
	for(i=0;i<=SEARCH_STEPS;i++)
	{
		t=SEARCH_STEPSIZE*(double)i;
		d2=dist2(t,z_p,r_p);
		if(d2<d2_min) t_min=t,d2_min=d2;
	}

	left=t_min-SEARCH_STEPSIZE;
	right=t_min+SEARCH_STEPSIZE;

	for(t=left;t<right;t+=SEARCH_EPSILON)
	{
		t_=t-floor(t);
		d2=dist2(t_,z_p,r_p);
		if(d2<d2_min) t_min=t_,d2_min=d2;
	}

	pos.z=z_func(t_min);
	r_t=r_func(t_min);
	pos.x=cos_a*r_t;
	pos.y=sin_a*r_t;
	makenormal(t_min,normal.z,r_n);
	normal.x=cos_a*r_n;
	normal.y=sin_a*r_n;*/

	dir.plane_project(normal);
	dir.normalize(); // project heading onto plane + normalize

	if(!connected)
	if(left==0||right==1)
	{
		vector edge=vector(-sin_a,cos_a,0);
		//v_diff(u,v).normalized();

		dir=-dir+edge*2*dir.dot(edge);
	}

}



void lathe::random_pos(vector &pos,vector &normal,vector &prev)
{
	double t,z1,r1,z2,r2,p;

	do
	{
		t=drand();

		z1=z_func(t-RAND_EPSILON);
		r1=r_func(t-RAND_EPSILON);
		z2=z_func(t+RAND_EPSILON);
		r2=r_func(t+RAND_EPSILON); // should check for out-of-bounds

		// generate probability for Monte Carlo routine
		p=sqrt((z1-z2)*(z1-z2)+(r1-r2)*(r1-r2))*(r1+r2)/2/(2*RAND_EPSILON)/area; //*PI
	} while(drand()>p);

	double z,r,a,sin_a,cos_a;

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

/*	double t,z,r,z_o,r_o,randarea,sin_a,cos_a,a,r_n;

	randarea=drand()*area;

	z_o=z_func(0);
	r_o=r_func(0);
	for(int i=1;i<=INT_STEPS;i++)
	{
		t=INT_STEPSIZE*(double)i;
		z=z_func(t);
		r=r_func(t);

//		randarea-=sqrt((z-z_o)*(z-z_o)+(r-r_o)*(r-r_o))*abs(r-r_o); // *PI
		randarea-=sqrt((z-z_o)*(z-z_o)+(r-r_o)*(r-r_o))*(r+r_o)/2; // FAKE

		if(randarea<0)
		{
			t-=drand()*INT_STEPSIZE; // spread uniformly in slice - bah
			z=z_func(t);
			r=r_func(t);
			a=drand()*2*PI;
			sin_a=sin(a);
			cos_a=cos(a);
			pos.x=cos_a*r;
			pos.y=sin_a*r;
			pos.z=z;
			makenormal(t,normal.z,r_n);
			normal.x=cos_a*r_n;
			normal.y=sin_a*r_n;
			break;
		}

		z_o=z;
		r_o=r;
	}*/
}

double lathe::_z_func(double t) { return(0); }
double lathe::_r_func(double t) { return(0); }

double lathe::z_func(double t)
{
	if(connected) t-=floor(t);
	return(_z_func(t));
}

double lathe::r_func(double t)
{
	if(connected) t-=floor(t);
	return(_r_func(t));
}

double lathe::z_diff(double t)
{
	double upper=t+DIFF_EPSILON;
	double lower=t-DIFF_EPSILON;

	if(!connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((z_func(upper)-z_func(lower))/(upper-lower));
}

double lathe::r_diff(double t)
{
	double upper=t+DIFF_EPSILON;
	double lower=t-DIFF_EPSILON;

	if(!connected)
	{
		if(upper>1) upper=1;
		if(lower<0) lower=0;
	}

	return((r_func(upper)-r_func(lower))/(upper-lower));
}

double lathe::dist2(double t,double z,double r)
{
	double dz,dr;

	dz=z_func(t)-z;
	dr=r_func(t)-r;
	return(dz*dz+dr*dr);
}

void lathe::make_normal(double t,double cos_a,double sin_a,vector &n)
{
	double z,r,sc;

	z=-r_diff(t);
	r=z_diff(t);
	sc=1/sqrt(z*z+r*r);

	n.x=r*sc*cos_a;
	n.y=r*sc*sin_a;
	n.z=z*sc;
}


para::para():lathe() { name="Paralemon"; ident="paralemon"; }
double para::_z_func(double t) { return(2*t-1); }
double para::_r_func(double t) { return(4*(0.25-(t-0.5)*(t-0.5))); }

peanut::peanut():lathe() { name="Peanut"; ident="peanut"; }
double peanut::_z_func(double t) { return(-cos(t*PI)); }
double peanut::_r_func(double t)
{
	double tt=-cos(t*PI); 
	double a=1-tt*tt-1.57/exp(tt*tt)+1.57/exp(1);
	if(a<0) return(0);
	return(2*sqrt(a));
}

bowl::bowl():lathe() { name="Bowl"; ident="bowl"; }
double bowl::_z_func(double t)
{
	return(
		t>0.55?(-0.8*cos((1-t)/0.45*PI/2)):
		t>0.45?(0.1*cos(PI/2-(t-0.45)/0.1*PI)):
		(-cos(t/0.45*PI/2))
	);
}
double bowl::_r_func(double t)
{
	return(
		t>0.55?(0.8*sin((1-t)/0.45*PI/2)):
		t>0.45?(0.1*sin(PI/2-(t-0.45)/0.1*PI)+0.9):
		(sin(t/0.45*PI/2))
	);
}



lsphere::lsphere():lathe() { name="Sphere (Alt)"; ident="lsphere"; }
double lsphere::_z_func(double t) { return(-cos(t*PI)); }
double lsphere::_r_func(double t) { return(sin(t*PI)); }

ltorus::ltorus():lathe() { name="Torus (Alt)"; ident="ltorus";}
double ltorus::_z_func(double t) { return(-0.4*cos(t*2*PI)); }
double ltorus::_r_func(double t) { return(0.4*sin(t*2*PI)+0.6); }

ufo::ufo():lathe() { name="UFO"; ident="ufo"; }
double ufo::_z_func(double t) { return(-0.4/tan(1.3)*tan(1.3*cos(t*2*PI))); }
double ufo::_r_func(double t) { return(0.4/tan(1.3)*tan(1.3*sin(t*2*PI))+0.6); }

ltop::ltop():lathe() { name="Top"; ident="top"; }
double ltop::_z_func(double t) { return(-1/tan(1.3)*tan(1.3*cos(t*PI))); }
double ltop::_r_func(double t) { return(1/tan(1.3)*tan(1.3*sin(t*PI))); }

barrel::barrel():lathe() { name="Barrel"; ident="barrel"; }
double barrel::_z_func(double t) { return(-0.7*cos(t*2*PI)); }
double barrel::_r_func(double t) { return(0.15*sin(t*2*PI)+0.55); }

spiral::spiral():lathe() { name="Spiral"; ident="spiral"; }
double spiral::_z_func(double t) { return(-0.4*t*cos(t*4*PI)); }
double spiral::_r_func(double t) { return(0.4*t*sin(t*4*PI)+0.6); }

spool::spool():lathe() { name="Spool"; ident="spool"; }
double spool::_z_func(double t) { return(-0.7*cos(t*PI)); }
double spool::_r_func( double t) { return(0.5*sin(t*PI)+0.35*sin(t*3*PI)+0.1*sin(t*5*PI)); }

bread::bread():lathe() { name="Bread"; ident="bread"; }
double bread::_z_func(double t) { return(-0.1*cos(t*2*PI)); }
double bread::_r_func(double t) { return(0.4*sin(t*2*PI)+0.6); }

test::test():lathe() { name="Test"; ident=0; }
double test::_z_func(double t)
{
	return(-0.1*cos(t*2*PI));
}

double test::_r_func(double t)
{
	return(0.4*sin(t*2*PI)+0.6);
}
