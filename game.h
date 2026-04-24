#ifndef _GAME_H_
#define _GAME_H_



#include "playfield.h"
#include "creatures.h"
#include "effects.h"


#define MAX_DT 0.1
#define DT_EPSILON 0.0001

#define CAMERA_REVOLVE_DIST 2
#define CAMERA_TRACK_DIST 1
#define CAMERA_SLIDE_DIST 2
#define CAMERA_FOLLOW_DIST 1
#define CAMERA_SCALE 300
#define CAMERA_CUTOFF 1000

#define COLLIDE_EXT 0 //0.1
#define COLLIDE_MULT 4



#define KEY_LEFT 0
#define KEY_RIGHT 1
#define KEY_UP 2
#define KEY_DOWN 3

#define CMODE_REVOLVING 0
#define CMODE_TRACKING 1
#define CMODE_SLIDING 2

#define INERTIA_NONE 0
#define INERTIA_LOW 1
#define INERTIA_MEDIUM 2
#define INERTIA_HIGH 3
#define INERTIA_SILLY 4

#define ANGLE_ABOVE 0
#define ANGLE_HIGH 1
#define ANGLE_MEDIUM 2
#define ANGLE_LOW 3



class game
{
	public:
	game();
	~game();

	virtual void setup(playfield *field);
	virtual void live(double dt,bool *keys);
	virtual void draw();
	virtual bool over();

	void calc_camera(vector pos,vector dir,vector normal);

	int camera_mode,camera_inertia,tracking_angle;

	char *ident;

	matrix cam,cam_current,cam_inv;

	playfield *p;

	double t;

	bool playing,firstframe;
	int score,next;

	double brightness;

	static lineparticles *lp;
};



#define APPLE_FLASH 1

class wormgame:public game
{
	public:
	wormgame();

	void live(double dt,bool *keys);
	bool over();

	playerworm w;
};

#define SIMPLE_APPLEFACTOR 0.008

class simplegame:public wormgame
{
	public:
	simplegame();

	void setup(playfield *field);

	void live(double dt,bool *keys);
	void draw();

	apple a;
};

#define HUNT_APPLEFACTOR 0.004

class huntgame:public wormgame
{
	public:
	huntgame();

	void setup(playfield *field);

	void live(double dt,bool *keys);
	void draw();

	skittler a;
};

class blindgame:public simplegame
{
	public:
	blindgame();

	void setup(playfield *field);

	void live(double dt,bool *keys);
};

#endif
