#ifndef _LATHE_H_
#define _LATHE_H_

#include "playfield.h"

class lathe:public playfield
{
	public:
	lathe();

	void prepare_lines(int detailmult);
	void prepare_dots(int detailmult);
	void precalc();

	void recalc(vector &pos,vector &dir,vector &normal,vector &prev);
	void random_pos(vector &pos,vector &normal,vector &prev);

	virtual double _z_func(double t);
	virtual double _r_func(double t);

	double z_func(double t);
	double r_func(double t);
	double z_diff(double t);
	double r_diff(double t);
	double dist2(double t,double z,double r);
	void make_normal(double t,double cos_a,double sin_a,vector &n);

	double stepsize;
	bool connected;

	double area;
};

class para:public lathe
{
	public:
	para();
	double _z_func(double t);
	double _r_func(double t);
};

class peanut:public lathe
{
	public:
	peanut();
	double _z_func(double t);
	double _r_func(double t);
};

class bowl:public lathe
{
	public:
	bowl();
	double _z_func(double t);
	double _r_func(double t);
};

class lsphere:public lathe
{
	public:
	lsphere();
	double _z_func(double t);
	double _r_func(double t);
};

class ltorus:public lathe
{
	public:
	ltorus();
	double _z_func(double t);
	double _r_func(double t);
};

class ufo:public lathe
{
	public:
	ufo();
	double _z_func(double t);
	double _r_func(double t);
};

class ltop:public lathe
{
	public:
	ltop();
	double _z_func(double t);
	double _r_func(double t);
};

class barrel:public lathe
{
	public:
	barrel();
	double _z_func(double t);
	double _r_func(double t);
};

class spiral:public lathe
{
	public:
	spiral();
	double _z_func(double t);
	double _r_func(double t);
};

class spool:public lathe
{
	public:
	spool();
	double _z_func(double t);
	double _r_func(double t);
};

class bread:public lathe
{
	public:
	bread();
	double _z_func(double t);
	double _r_func(double t);
};

class test:public lathe
{
	public:
	test();
	double _z_func(double t);
	double _r_func(double t);
};

#endif
