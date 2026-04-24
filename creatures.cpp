#include "creatures.h"
#include "game.h"
#include "random.h"

#include <OpenGL/gl.h>

#define MAX(a,b) ((a)>(b)?(a):(b))



// worm

worm::worm()
{
}

worm::~worm()
{
}

void worm::place(playfield *p)
{
	head=tail=0;

	joint[0].pos=pos;
	joint[0].normal=normal;
	joint[0].l=0;

	dead=false;
	deathtimer=0;
}

#define queueoffs(n,offs) ((n)+(offs)&(MAXLEN-1))

void worm::live(double dt,bool *keys,playfield *p)
{
	if(dead)
	{
		deathtimer+=dt;

		double newlen=deathtimer*DEATH_SPEED;
		vector prevpos;
		int start_i=-1;
		double start_l;

		if(findpoint(deathlength,&prevpos,0,&start_i,&start_l))
		while(deathlength<newlen)
		{
			vector pos,normal;

			deathlength+=0.03;

			if(findpoint(deathlength,&pos,&normal,&start_i,&start_l))
			for(int i=0;i<2;i++)
			game::lp->add_randomly_rotating_line(pos,prevpos,normal,0.4,0.4,DEATH_FRAGMENTTIME,2,1,1);

			prevpos=pos;
		}
	}
}

void worm::draw(vector view)
{
	double l=0;

	glBegin(GL_LINE_STRIP);

	int i=queueoffs(head,1);
	do {
		i=queueoffs(i,-1);
		l+=joint[i].l;

		if(!dead||l>deathtimer*DEATH_SPEED)
		{
			float c=view.dot(joint[i].normal);
			if(c<0) c=0;
			c*=(sin(l*60)+1)/2;
			c=(c+0.33)/1.33;

			glColor3f(c,c/2,c/2);
			glVertex3f(joint[i].pos.x,joint[i].pos.y,joint[i].pos.z);
		}
	} while(i!=tail);

	glEnd();
}

void worm::setnext(vector r,playfield *p)
{
	double sumlen;
	vector pos_old=pos;

	pos=r;
	p->recalc(pos,dir,normal,prev); // reposition + get new normal

	head=queueoffs(head,1);
	if(head==tail) tail=queueoffs(tail,1);

	joint[head].pos=pos;
	joint[head].normal=normal;
	joint[head].l=(pos-pos_old).length();

	sumlen=0;
	for(int i=head;i!=tail;i=queueoffs(i,-1)) // run through queue and chop off end
	{
		sumlen+=joint[i].l;
		if(sumlen>=len)
		{
			double l=sumlen-len;

			tail=queueoffs(i,-1);
			joint[tail].pos=(joint[i].pos*l+joint[tail].pos*(joint[i].l-l))/joint[i].l;
			//p->recalc(worm[tail].pos,worm[tail].normal);
			joint[i].l-=l; // shorten last segment
			break;
		}
	}
}

bool worm::findpoint(double x,vector *pos,vector *normal,int *start_i,double *start_l)
{
	int i=head;
	double l=0;

	if(x>len) return false;

	if(start_i&&*start_i>=0)
	{
		i=*start_i;
		if(start_l) l=*start_l;
	}

	do {
		int next=queueoffs(i,-1);
		double curr_l=joint[i].l;

		if(l+curr_l>x) // found
		{
			double offs_l=x-l;

			if(pos) *pos=(joint[i].pos*(curr_l-offs_l)+joint[next].pos*offs_l)/curr_l;
			if(normal) *normal=(joint[i].normal*(curr_l-offs_l)+joint[next].normal*offs_l).normalized();

			if(start_i) *start_i=i;
			if(start_l) *start_l=l;

			return true;
		}

		l+=curr_l;
		i=next;
	} while(i!=tail);

	return false;
}


void worm::die()
{
	dead=true;
	deathlength=0;

//	deathtimer=DEATH_TIME;

/*	vector_f pos=vector_f(joint[head].pos.x,joint[head].pos.y,joint[head].pos.z);
	for(int i=0;i<DEATH_DOTS;i++)
	{
		dotpos[i]=pos;
		dotvel[i]=vector_f::rand_sphere().normalized()*DEATH_VEL;
	}*/
	//play_sample(&death,255,127,1000,0);
}

bool worm::collide(
const vector &r1,const vector &r2,const vector &r3,const vector &r4,
const vector &n1,const vector &n2,const vector &n3,const vector &n4)
{
	if((n1+n2).dot(n3+n4)<0) return(false);

	vector u,v,w;
	vector r;
	vector d;
	double l2;

	u=r2-r1; // u base: p_1 -> p_2
	v=r4-r3; // v base: p_3 -> p_4
	w=(u*v).normalized(); // w base: u x v

	r=r3-r1;// change origo for p_3 and store in r
	l2=MAX(u.lengthsq(),v.lengthsq()); // squared length of longer segment

	// / u.x v.x w.x \ / r.x' \   / r.x \
	// | u.y v.y w.y | | r.y' | = | r.y |
	// \ u.z v.z w.z / \ r.z' /   \ r.z /

	double det=1/(
		u.x*v.y*w.z-u.x*v.z*w.y
		+u.y*v.z*w.x-u.y*v.x*w.z
		+u.z*v.x*w.y-u.z*v.y*w.x
	);

	vector a;

	a.x=(
		(v.y*w.z-v.z*w.y)*r.x
		-(v.x*w.z-v.z*w.x)*r.y
		+(v.x*w.y-v.y*w.x)*r.z
	)*det;

	a.y=(
		-(u.y*w.z-u.z*w.y)*r.x
		+(u.x*w.z-u.z*w.x)*r.y
		-(u.x*w.y-u.y*w.x)*r.z
	)*det;

	a.z=(
		(u.y*v.z-u.z*v.y)*r.x
		-(u.x*v.z-u.z*v.x)*r.y
		+(u.x*v.y-u.y*v.x)*r.z
	)*det;

/*	v.x/=u.x; w.x/=u.x; r.x/=u.x; // u.x=1;
	v.y-=v.x*u.y; w.y-=w.x*u.y; r.y-=r.x*u.y; // u.y=0;
	v.z-=v.x*u.z; w.z-=w.x*u.z; r.z-=r.x*u.z; // u.z=0;

	w.y/=v.y; r.y/=v.y; // v.y=1;
	w.z-=w.y*v.z; r.z-=r.y*v.z; // v.z=1;

	r.y-=r.z*w.y; // w.y=0;
	r.z-=r.z*w.x; // w.x=0;

	r.x-=r.y*v.x; // v.x=0;*/

	if(a.x<0||a.x>1) return(false); // intersect is outside u vector
	if(a.y>0||a.y<-1) return(false); // intersect is outside v vector

//	d=(r1*(1-r.x)+r2*r.x)-(r3*(1+r.y)-r4*r.y); // distance between intersection points
	d=r1+(r2-r1)*a.x-(r3+(r4-r3)*a.y);

	if(d.lengthsq()>l2*COLLIDE_MULT) return(false);

	return(true);
}



// playerworm

playerworm::playerworm():worm()
{
}

playerworm::~playerworm()
{
}

void playerworm::place(playfield *p)
{
	p->random(pos,dir,normal,prev); // random start pos

	len=WORM_STARTLENGTH;

	worm::place(p);
}

void playerworm::live(double dt,bool *keys,playfield *p)
{
	worm::live(dt,keys,p);

	if(deathtimer) return;

	double v,a;

	if(keys[KEY_LEFT]&&keys[KEY_RIGHT]) a=(3*drand()-1.5)*WORM_TURN;
	else if(keys[KEY_LEFT]) a=WORM_TURN;
	else if(keys[KEY_RIGHT]) a=-WORM_TURN;
	else a=0;

	if(keys[KEY_DOWN]) a*=2.0;
	if(keys[KEY_UP]) v=2.0*WORM_SPEED;
	else v=WORM_SPEED;

/*	if(c->joystick&&num_joysticks>0&&!(joy[0].flags&JOYFLAG_CALIBRATE))
	{
		poll_joystick();

		a=-2.0*WORM_TURN*double(joy[0].stick[0].axis[0].pos)/128.0;

		v=WORM_SPEED;
		for(int i=0;i<joy[0].num_buttons;i++)
		if(joy[0].button[i].b) { v*=2.0; break; }
	}
	else
	{
		if(key[c->key_left]&&key[c->key_right]) a=(3*drand()-1.5)*WORM_TURN;
		else if(key[c->key_left]) a=WORM_TURN;
		else if(key[c->key_right]) a=-WORM_TURN;
		else a=0;

		if(key[c->key_down]) a*=2.0;
		if(key[c->key_up]) v=2.0*WORM_SPEED; else v=WORM_SPEED;
	}*/

	dir=quaternion::rotation(a*dt,normal).apply_rotation(dir);

	vector pos_old=pos,normal_old=normal;

	setnext(pos+dir*v*dt,p);

	// should be: move(pos,dir,v*dt,p)

	int n;
	if(tail!=queueoffs(head,-1)) // 1 segment worm - special case
	for(int i=queueoffs(head,-2);i!=tail;i=n)
	{
		n=queueoffs(i,-1);

		if(collide(pos,pos_old,joint[n].pos,joint[i].pos,
		normal,normal_old,joint[n].normal,joint[i].normal))
		{
			die();
			break;
		}
	}
}



// skittler

skittler::skittler()
{
	w=0;
//	size=0.04;
//	vel=0.4;
//	turn=1;
}

skittler::~skittler()
{
}

#define VEL_MIN 0.3
#define VEL_MAX 0.75
#define SIZE_MIN 0.025
#define SKITTLER_SIZE_MAX 0.05
#define TURN_MIN 0.5
#define TURN_MAX 2.5
#define SPEED_OFFSET 1.3

void skittler::place(playfield *p)
{
	p->random(pos,dir,normal,prev);

	double r1=drand(),r2=drand(),r3=drand();
	vel=r1*r1*(VEL_MAX-VEL_MIN)+VEL_MIN;
	size=r2*(SKITTLER_SIZE_MAX-SIZE_MIN)+SIZE_MIN;
	turn=r3*r3*(TURN_MAX-TURN_MIN)+TURN_MIN;

	points=int(100*(r1+0.6)/(r2+1.1)*(r3+0.6)/1.6*1.1/1.6);
}

void skittler::live(double dt,playfield *p)
{
	if(vel)
	{
		vector pos_old=pos,normal_old=normal;

		pos+=dir*vel*dt;
		p->recalc(pos,dir,normal,prev); // reposition + get new normal

		int n;
		if(w)
		for(int i=w->head;i!=w->tail;i=n)
		{
			n=queueoffs(i,-1);

			if(worm::collide(pos,pos_old,w->joint[n].pos,w->joint[i].pos,
			normal,normal_old,w->joint[n].normal,w->joint[i].normal))
			{
				vector bounce=((w->joint[n].pos-w->joint[i].pos)
				*(w->joint[n].normal+w->joint[i].normal)).normalized();

				dir-=bounce*2*bounce.dot(dir);
				pos=pos_old;

				break;
			}
		}

		a+=(2*drand()-1)*20*dt; // adjustable?
		dir+=((dir)*sin(a)+(dir*normal)*cos(a))*turn*dt;
		dir.normalize(); // renormalize
	}
}

void skittler::draw(vector view)
{
	float c1=view.dot(normal);
	if(c1<0) c1=0;

	vector drawpos=pos+dir*size*(vel*SPEED_OFFSET);

	glBegin(GL_LINES);

	for(int i=0;i<16;i++)
	{
		vector r=vector::rand_sphere().normalized();
		if(r.dot(normal)<0) r-=normal*2*r.dot(normal);

		vector tip=pos+r*size;
		float c2=view.dot(r);

		float c=(3*c1+c2)/4;

		glColor3f((c+0.33)/1.33,c/4,c/4);
		glVertex3f(drawpos.x,drawpos.y,drawpos.z);
		glVertex3f(tip.x,tip.y,tip.z);
	}

	glEnd();

/*	col=g->project(pos+dir*size*(vel*SPEED_OFFSET),normal,x_o,y_o); //,g->p->onesided);

	if(col>=0)
	for(int i=0;i<16;i++)
	{
		vector r;
		double sc;
		int col2;

		r=vector::rand_sphere().normalized();
		if(r.dot(normal)<0) r-=normal*2*r.dot(normal);

		col2=g->project(pos+r*size,r,x,y);

		if(col2>=0)
		{
			if(c->antialias)
			antialias_line8(bm,x,y,x_o,y_o,192,(3*col+col2)/4);
			else line(bm,x,y,x_o,y_o,(3*col+col2)/4+192);
		}
	}*/
}

void skittler::die()
{
	vector drawpos=pos+dir*size*(vel*SPEED_OFFSET);

	for(int i=0;i<32;i++)
	{
		vector r=vector::rand_sphere().normalized();
		if(r.dot(normal)<0) r-=normal*2*r.dot(normal);
		vector tip=pos+r*size;

		game::lp->add_line(drawpos,tip,r,vector(0,0,0),0.5,2,0.5,0.5);
	}
}

bool skittler::collide(const vector &other,const vector &other_normal)
{
	return(
		(other-pos).lengthsq()<size*size
		&&normal.dot(other_normal)>0
	);
}



// apple

apple::apple():skittler()
{
	vel=0;
	size=0.03;
}

apple::~apple()
{
}

void apple::place(playfield *p)
{
	p->random(pos,dir,normal,prev);
	points=14+grand()%(42-14+1);
}
