#ifndef _TORUS_H_
#define _TORUS_H_

#include "playfield.h"

class torus:public playfield
{
	public:
	torus();

	void prepare_lines(int detailmult);
	void recalc_pos(vector &pos,vector &normal,vector &prev);
	void random_pos(vector &pos,vector &normal,vector &prev);

	double majorrad,minorrad;
};

class normtorus:public torus
{
	public:
	normtorus();
};

class thicktorus:public torus
{
	public:
	thicktorus();
};

class thintorus:public torus
{
	public:
	thintorus();
};

#endif
