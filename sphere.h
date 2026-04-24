#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "playfield.h"

class sphere:public playfield
{
	public:
	sphere();

	void prepare_lines(int detailmult);
	void recalc_pos(vector &pos,vector &normal,vector &prev);
	void random_pos(vector &pos,vector &normal,vector &prev);
};

#endif
