#ifndef _CREATURES_H_
#define _CREATURES_H_

#include "vector.h"



#define MAXLEN 65536 // big. big is good.

#define WORM_SPEED 0.6
#define WORM_TURN 2.8
#define WORM_STARTLENGTH 0.3

#define DEATH_SPEED 5
#define DEATH_FRAGMENTTIME 2

#define DEATH_DOTS 256
#define DEATH_TIME 1
#define DEATH_VEL 1
#define DEATH_DRAG 3



class playfield;

class worm
{
	public:
	worm();
	~worm();

	void place(playfield *p);
	void live(double dt,bool *keys,playfield *p);
	void draw(vector view);
	void die();

	void setnext(vector r,playfield *p);
	bool findpoint(double x,vector *pos,vector *normal,int *start_i=0,double *start_l=0);

	static bool collide(
	const vector &r1,const vector &r2,const vector &r3,const vector &r4,
	const vector &n1,const vector &n2,const vector &n3,const vector &n4);

	vector pos,dir,normal,prev;

	struct wormjoint
	{
		vector pos;
		vector normal;
		double l;
	} joint[MAXLEN];

	bool dead;

	int head,tail;
	double len;
	double deathtimer,deathlength;
};

class playerworm:public worm
{
	public:
	playerworm();
	~playerworm();

	void place(playfield *p);
	void live(double dt,bool *keys,playfield *p);
};

class skittler
{
	public:
	skittler();
	~skittler();

	void place(playfield *p);
	void live(double dt,playfield *p);
	void draw(vector view);
	void die();
	bool collide(const vector &other,const vector &other_normal);

	worm *w;

	double size,vel,turn;

	vector pos,dir,normal,prev;
	int points;
	double a;
};

class apple:public skittler
{
	public:
	apple();
	~apple();

	void place(playfield *p);
};


#endif
