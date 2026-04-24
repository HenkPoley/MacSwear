#include <time.h>
#include <string.h>

#include "game.h"



lineparticles *game::lp=0;



game::game()
{
	ident="";

	camera_mode=CMODE_REVOLVING;
	camera_inertia=INERTIA_NONE;
	tracking_angle=ANGLE_ABOVE;

	if(!lp) lp=new lineparticles;

	playing=false;
}

game::~game()
{
}



void game::setup(playfield *field)
{
	p=field;
	t=0;
	score=0;
	next=0;
	firstframe=true;
	brightness=1;

	lp->clear();
}

void game::live(double dt,bool *keys)
{
	t+=dt;
	brightness-=(brightness-1)*4*dt;

	lp->live(dt);
}

void game::draw()
{
	vector view=cam.position().normalized();

	p->draw(view,brightness);
	lp->draw(view);
}

bool game::over()
{
	return false;
}

void game::calc_camera(vector pos,vector dir,vector normal)
{
	switch(camera_mode)
	{
		case CMODE_REVOLVING:
			cam=matrix(dir*normal,dir,normal,vector(0,0,0))
			   *matrix::translation(0,0,CAMERA_REVOLVE_DIST);
		break;

		case CMODE_TRACKING:
		{
			static double trackingangles[]={PI/2,PI/3,PI/4,PI/6};
			double a=trackingangles[tracking_angle];
			double sin_a=sin(a),cos_a=cos(a);

			vector forward=normal*sin_a-dir*cos_a;
			vector up=dir*sin_a+normal*cos_a;

			cam=matrix::translation(0,0,CAMERA_FOLLOW_DIST);
			cam=matrix(up*forward,up,forward,vector(0,0,0))*cam;
			cam=matrix::translation(pos)*cam;
		}
		break;

		case CMODE_SLIDING:
			cam=matrix(dir*normal,dir,normal,vector(0,0,0));
			cam*=matrix::translation(cam.x_axis().dot(pos),cam.y_axis().dot(pos),CAMERA_SLIDE_DIST);

			//cam=matrix::translation(pos)*cam;
			//cam=matrix::translation(cam.z_axis()*(-cam.position().dot(cam.z_axis())+CAMERA_SLIDE_DIST))*cam;
		break;
	}

	if(firstframe||camera_inertia==INERTIA_NONE)
	{
		cam_current=cam;

		firstframe=false;
	}
	else
	{
		// gaah.

		//static double smoothfactor[]={0,10,6,3,1};
		//double fac=smoothfactor[camera_inertia];

		vector x_axis=cam.x_axis();
		vector y_axis=cam.y_axis();
		vector z_axis=cam.z_axis();
		vector pos=cam.position();

		vector x_axis_old=cam_current.x_axis();
		vector y_axis_old=cam_current.y_axis();
		vector z_axis_old=cam_current.z_axis();
		vector pos_old=cam_current.position();

		vector x_axis_new,y_axis_new,z_axis_new,pos_new;

		x_axis_new=x_axis;
		y_axis_new=y_axis;
		z_axis_new=z_axis;
		pos_new=pos;

		cam_current=matrix(x_axis_new,y_axis_new,z_axis_new,pos_new);

//		double fac=smoothfactor[c->smoothing];

/*		double speed=10,length;
		vector delta;

		delta=up-y_axis;
		length=delta.length();
		if(length>speed*dt) delta*=speed*dt/length;
		y_axis+=delta;
		y_axis.normalize();

		delta=toward-z_axis;
		length=delta.length();
		if(length>speed*dt) delta*=speed*dt/length;
		z_axis+=delta;
		z_axis-=y_axis*y_axis.dot(z_axis);
		z_axis.normalize();*/
	}

	cam_inv=cam_current.fast_inverse();

}




// wormgame

wormgame::wormgame():game()
{
}

void wormgame::live(double dt,bool *keys)
{
	calc_camera(w.pos,w.dir,w.normal);
	game::live(dt,keys);
}

bool wormgame::over()
{
	return w.dead&&w.deathtimer>w.len/DEATH_SPEED+DEATH_FRAGMENTTIME;
}




// simplegame

simplegame::simplegame():wormgame()
{
	ident="normal";
}

void simplegame::setup(playfield *field)
{
	wormgame::setup(field);

	w.place(p);
	a.place(p);
	next=a.points;
}

void simplegame::live(double dt,bool *keys)
{
	w.live(dt,keys,p);
	a.live(dt,p);

	if(!w.dead&&a.collide(w.pos,w.normal))
	{
		score+=a.points;
		w.len+=SIMPLE_APPLEFACTOR*double(a.points);
		brightness+=APPLE_FLASH;
		a.die();
		a.place(p);
		next=a.points;
	}

	wormgame::live(dt,keys);
}

void simplegame::draw()
{
	vector view=cam.position().normalized();

	wormgame::draw();

	w.draw(view);
	a.draw(view);
}



// huntgame

huntgame::huntgame():wormgame()
{
	ident="hunt";
	a.w=&w;
}

void huntgame::setup(playfield *field)
{
	wormgame::setup(field);

	w.place(p);
	a.place(p);
	next=a.points;
}

void huntgame::live(double dt,bool *keys)
{
	w.live(dt,keys,p);
	a.live(dt,p);

	if(!w.dead&&a.collide(w.pos,w.normal))
	{
		score+=a.points;
		w.len+=HUNT_APPLEFACTOR*double(a.points);
		brightness+=APPLE_FLASH;
		a.die();
		a.place(p);
		next=a.points;
	}

	wormgame::live(dt,keys);
}

void huntgame::draw()
{
	vector view=cam.position().normalized();

	wormgame::draw();

	w.draw(view);
	a.draw(view);
}



// blindgame

blindgame::blindgame()
{
	ident="blind";
}

void blindgame::setup(playfield *field)
{
	simplegame::setup(field);

	brightness=0;
}

void blindgame::live(double dt,bool *keys)
{
	simplegame::live(dt,keys);

	if(w.deathtimer) brightness=1-exp(-(w.deathtimer-drand()*2)/2);
	else brightness=0;
	if(brightness<0) brightness=0;
}
