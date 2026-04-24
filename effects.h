#ifndef _EFFECTS_H_
#define _EFFECTS_H_

#include "vector.h"

#define STAR_MAXNUM 16384
#define STAR_MAXSPEED 0.8
#define STAR_TRAIL 0.1

#define STARS_NONE 0
#define STARS_SPARSE 1
#define STARS_MEDIUM 2
#define STARS_DENSE 3
#define STARS_SILLY 4

#define STARSPEED_STATIC 0
#define STARSPEED_SLOW 1
#define STARSPEED_MEDIUM 2
#define STARSPEED_FAST 3
#define STARSPEED_SILLY 4

class starfield
{
	public:
	starfield();
	~starfield();

	void live(double dt);
	void draw(vector campos);

	void set_num(int n,int max);
	void set_speed(int v,int max);

	int num;
	double starspeed;

	struct star
	{
		vector_f pos;
		uint8 r,g,b,a;
		vector_f tail;
		uint8 bl1,bl2,bl3,a2;
	} stars[STAR_MAXNUM];
};



#define LINE_CHUNK 1024

class lineparticles
{
	public:
	lineparticles();
	~lineparticles();

	void live(double dt);
	void draw(vector view);

	void add_line(vector p1,vector p2,vector v,vector rot,float ttl,float r,float g,float b);
	void add_randomly_rotating_line(vector p1,vector p2,vector n,float v,float rot,float ttl,float r,float g,float b);

	void clear();

	int num_lines,total_lines;

	struct line
	{
		vector_f pos;
		vector_f dir;
		vector_f axis;
		vector_f vel;
		float dadt;
		float t;
		float timetolive;
		uint16 r,g,b;
	} *lines;

	struct vertex
	{
		vector_f p;
		uint8 r,g,b,a;
	} *vertices;
};

#endif
