#ifndef _EXPLICIT_H_
#define _EXPLICIT_H_

#include "playfield.h"

class xplicit:public playfield
{
	public:
	xplicit();

	void prepare_lines(int detailmult);
	void prepare_dots(int detailmult);
	void precalc();

	void recalc(vector &pos,vector &dir,vector &normal,vector &prev);
	void random_pos(vector &pos,vector &normal,vector &prev);

	virtual vector _func(double u,double v);

	vector func(double u,double v);
	vector u_diff(double u,double v);
	vector v_diff(double u,double v);
	double dist2(double u,double v,const vector &r);
	double dist2_u_diff(double u,double v,const vector &r);
	double dist2_v_diff(double u,double v,const vector &r);
	void make_normal(double u,double v,vector &n);

//	double u_stepsize,v_stepsize;
	int u_count_mult,u_count_div;

	bool u_connected,v_connected;
};

class etest:public xplicit
{
	public:
	etest();
	vector _func(double u,double v);
};

class moebius:public xplicit
{
	public:
	moebius();
	vector _func(double u,double v);
};

class mtwist:public xplicit
{
	public:
	mtwist();
	vector _func(double u,double v);
};

class klein:public xplicit
{
	public:
	klein();
	vector _func(double u,double v);
};

class trefoil:public xplicit
{
	public:
	trefoil();
	vector _func(double u,double v);
};

#endif
