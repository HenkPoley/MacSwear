#include <OpenGL/gl.h>

#include "effects.h"



// starfield

starfield::starfield()
{
	for(int i=0;i<STAR_MAXNUM;i++)
	{
		stars[i].pos=vector_f::rand_sphere();
		stars[i].bl1=stars[i].bl2=stars[i].bl3=0;
	}

	num=0;
	starspeed=0;
}

starfield::~starfield()
{
}

void starfield::set_num(int n,int max)
{
	num=(n*STAR_MAXNUM)/max;
}

void starfield::set_speed(int v,int max)
{
	starspeed=double(v*v*STAR_MAXSPEED)/(max*max);
}

void starfield::live(double dt)
{
	vector_f vel=vector_f(0,0,starspeed);
	vector_f dpos=vel*dt;

	for(int i=0;i<num;i++)
	{
		stars[i].pos+=dpos;

		float dist=stars[i].pos.length();

		if(dist>1)
		{
			do
			{
				stars[i].pos.x=2*drand()-1;
				stars[i].pos.y=2*drand()-1;
			} while(stars[i].pos.x*stars[i].pos.x+stars[i].pos.y*stars[i].pos.y>1);
			stars[i].pos.z=-sqrt(1-stars[i].pos.x*stars[i].pos.x-stars[i].pos.y*stars[i].pos.y);
			dist=1;
		}

		float col=1-dist;

		stars[i].tail=stars[i].pos-vel*col*STAR_TRAIL;
		stars[i].r=stars[i].g=stars[i].b=int(col*255);
	}
}

void starfield::draw(vector campos)
{
	int size,numelem;
	GLuint type;

	if(starspeed)
	{
		size=sizeof(star)/2;
		numelem=2*num;
		type=GL_LINES;
	}
	else
	{
		size=sizeof(star);
		numelem=num;
		type=GL_POINTS;
	}

	glEnable(GL_LINE_SMOOTH);

	glVertexPointer(3,GL_FLOAT,size,&stars[0].pos);
	glColorPointer(3,GL_UNSIGNED_BYTE,size,&stars[0].r);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glLockArraysEXT(0,num);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(campos.x,campos.y,campos.z);
	glScalef(10,10,10);

	glDrawArrays(type,0,numelem);

	glPopMatrix();

	glUnlockArraysEXT();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_LINE_SMOOTH);
}



// lineparticles

lineparticles::lineparticles()
{
	num_lines=0;
	total_lines=LINE_CHUNK;
	lines=new line[total_lines];
	vertices=new vertex[total_lines*2];
}

lineparticles::~lineparticles()
{
	delete lines;
	delete vertices;
}

void lineparticles::live(double dt)
{
	for(int i=0;i<num_lines;i++)
	{
		lines[i].t+=dt;
		float t=lines[i].t;
		float ttl=lines[i].timetolive;

		if(t>ttl)
		{
			lines[i].r=lines[i].g=lines[i].b=0;
		}
		else
		{
			if(lines[i].dadt)
			{
				quaternion_f q=quaternion_f::rotation(lines[i].dadt*dt,lines[i].axis);
				lines[i].dir=q.apply_rotation(lines[i].dir);
			}
			lines[i].pos+=lines[i].vel*dt;

			vertices[2*i].p=lines[i].pos+lines[i].dir;
			vertices[2*i+1].p=lines[i].pos-lines[i].dir;

			float sc=(ttl-t)/ttl;
			int r=int(float(lines[i].r)*sc);
			int g=int(float(lines[i].g)*sc);
			int b=int(float(lines[i].b)*sc);
			if(r>255) r=255;
			if(g>255) g=255;
			if(b>255) b=255;

			vertices[2*i].r=vertices[2*i+1].r=r;
			vertices[2*i].g=vertices[2*i+1].g=g;
			vertices[2*i].b=vertices[2*i+1].b=b;
		}
	}
}

void lineparticles::draw(vector view)
{
	glEnable(GL_LINE_SMOOTH);

	glVertexPointer(3,GL_FLOAT,sizeof(struct vertex),&vertices[0].p);
	glColorPointer(3,GL_UNSIGNED_BYTE,sizeof(struct vertex),&vertices[0].r);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glLockArraysEXT(0,num_lines*2);
	glDrawArrays(GL_LINES,0,num_lines*2);
	glUnlockArraysEXT();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_LINE_SMOOTH);
}

void lineparticles::add_line(vector p1,vector p2,vector v,vector rot,float ttl,float r,float g,float b)
{
	int i;
	for(i=0;i<num_lines;i++) if(!lines[i].r&&!lines[i].g&&!lines[i].b) break;

	if(i==num_lines)
	{
		if(num_lines==total_lines)
		{
			total_lines+=LINE_CHUNK;
			line *newlines=new line[total_lines];

			for(int i=0;i<num_lines;i++) newlines[i]=lines[i];

			delete lines;
			lines=newlines;

			delete vertices;
			vertices=new vertex[total_lines*2];
		}
		num_lines++;
	}

	float dadt=rot.abs();
	vector_f mid=vector_f(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z)/2;
	vector_f dir=vector_f(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z)/2;
	vector_f axis=vector_f(rot.x,rot.y,rot.z)/dadt;

	lines[i].pos=mid;
	lines[i].dir=dir;
	lines[i].axis=axis;
	lines[i].vel=vector_f(v.x,v.y,v.z);
	lines[i].dadt=dadt;
	lines[i].t=0;
	lines[i].timetolive=ttl;
	lines[i].r=int(r*255);
	lines[i].g=int(g*255);
	lines[i].b=int(b*255);
}

void lineparticles::add_randomly_rotating_line(vector p1,vector p2,vector n,float v,float rot,float ttl,float r,float g,float b)
{
	vector f1=vector::rand_sphere();
	if(f1.dot(n)<0) f1-=n*2*f1.dot(n);

	vector f2=vector::rand_sphere();
	if(f2.dot(n)<0) f2-=n*2*f2.dot(n);

	vector vel=(f1+f2)*v;
	vector w=(f1-f2)*(p1-p2)/(2*(p1-p2).abs2())*rot;

	add_line(p1,p2,vel,w,ttl,r,g,b);
}

void lineparticles::clear()
{
	num_lines=0;
}