#ifndef _PLAYFIELD_H_
#define _PLAYFIELD_H_

#include <math.h>

#include "vector.h"


#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define SQRT_2 1.4142135623730950488016887242097

#define MAX_STRLEN 1024
#define MAX_NAMELEN 128

#define ONESIDED_NONE 0
#define ONESIDED_MOEBIUS 1
#define ONESIDED_KLEIN 2

#define DETAIL_VERYLOW 0
#define DETAIL_LOW 1
#define DETAIL_MEDIUM 2
#define DETAIL_HIGH 3
#define DETAIL_VERYHIGH 4
#define DETAIL_SILLY 4

#define VISMODE_WIREFRAME 0
#define VISMODE_DOTS 1


struct vertex
{
	vector_f pos;
	uint8 r,g,b,a;
	vector_f n;
	float dummy;
};

struct line
{
	int a,b;
};

class playfield
{
	public:
	playfield();
	~playfield();

	virtual void prepare(int detail,int mode);
	virtual void prepare_lines(int detailmult);
	virtual void prepare_dots(int detailmult);
	virtual void unprepare();

	void alloc_arrays(int nvertices,int nlines);

	virtual void recalc(vector &pos,vector &dir,vector &normal,vector &prev);
	virtual void random(vector &pos,vector &dir,vector &normal,vector &prev);

	virtual void recalc_pos(vector &pos,vector &normal,vector &prev);
	virtual void random_pos(vector &pos,vector &normal,vector &prev);

	virtual void draw(vector view,double brightness);

	void firstprogress();
	void showprogress(int prog,int end);

	char *name,*ident;
	int onesided;

	int currdetail,vismode;
	bool prepared;

	int num_vertices,num_lines;
	vertex *vertices;
	line *lines;
};

#endif
