#include <OpenGL/gl.h>

#include "playfield.h"
#include "game.h"



//starfield s;



playfield::playfield()
{
	name="Unnamed";
	ident=0;
	onesided=ONESIDED_NONE;

	vismode=-1;
	currdetail=-1;

	num_vertices=0;
	num_lines=0;
	vertices=0;
	lines=0;
}

playfield::~playfield()
{
	unprepare();
}

void playfield::prepare(int detail,int mode)
{
	if(detail==currdetail&&mode==vismode) return;

	currdetail=detail;
	vismode=mode;

	static int detailmultipliers[]={8,12,16,24,32};
	int detailmult=detailmultipliers[detail];

	switch(vismode)
	{
		case VISMODE_WIREFRAME: prepare_lines(detailmult); break;
		case VISMODE_DOTS: prepare_dots(detailmult); break;
	}
}

void playfield::prepare_lines(int detailmult)
{
}

void playfield::prepare_dots(int detailmult)
{
	vector dummy;

	alloc_arrays(128*detailmult*detailmult,0);

	firstprogress();

	for(int i=0;i<num_vertices;i++)
	{
		vector pos,normal;
		random_pos(pos,normal,dummy);
		vertices[i].pos=vector_f(pos.x,pos.y,pos.z);
		vertices[i].n=vector_f(normal.x,normal.y,normal.z);

		if((i&127)==0) showprogress(i,num_vertices);
	}
}

void playfield::unprepare()
{
	delete[] vertices;
	delete[] lines;
	vertices=0;
	lines=0;
	num_vertices=0;
	num_lines=0;
}

void playfield::alloc_arrays(int nvertices,int nlines)
{
	num_vertices=nvertices;
	num_lines=nlines;
	vertices=new vertex[nvertices];
	if(num_lines) lines=new line[nlines];
}

void playfield::recalc(vector &pos,vector &dir,vector &normal,vector &prev)
{
	recalc_pos(pos,normal,prev);

	dir.plane_project(normal);
	dir.normalize(); // project heading onto plane + normalize
}

void playfield::random(vector &pos,vector &dir,vector &normal,vector &prev)
{
	random_pos(pos,normal,prev);

	dir=vector::rand_sphere(); // random heading
	dir.plane_project(normal); // project onto surface
	dir.normalize();
}

void playfield::recalc_pos(vector &pos,vector &normal,vector &prev) {}

void playfield::random_pos(vector &pos,vector &normal,vector &prev) {}

void playfield::draw(vector view,double brightness)
{
	vector_f view_f=vector_f(view.x,view.y,view.z);

	for(int i=0;i<num_vertices;i++)
	{
		float dot=view_f.dot(vertices[i].n);

		if(dot<0)
		if(onesided) dot=-dot;
		else dot=0;

		int c=int(brightness*(31+dot*(128-32)));
		if(c>255) c=255;

		vertices[i].r=0;
		vertices[i].g=c;
		vertices[i].b=0;
	}

	glEnable(GL_LINE_SMOOTH);

	glVertexPointer(3,GL_FLOAT,sizeof(struct vertex),&vertices[0].pos);
	glColorPointer(3,GL_UNSIGNED_BYTE,sizeof(struct vertex),&vertices[0].r);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glLockArraysEXT(0,num_vertices);

	if(num_lines) glDrawElements(GL_LINES,num_lines*2,GL_UNSIGNED_INT,lines);
	glDrawArrays(GL_POINTS,0,num_vertices);

	glUnlockArraysEXT();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_LINE_SMOOTH);
}



void playfield::firstprogress()
{
/*	textout_centre(screen,victor_12,"Preparing...",SCREEN_W/2-1,SCREEN_H/2-12,1);

	int x=(SCREEN_W-PROGRESS_WIDTH)/2;
	int y=SCREEN_H/2+1;

	rect(screen,x,y,x+PROGRESS_WIDTH-1,y+PROGRESS_HEIGHT-1,1);*/
}

void playfield::showprogress(int prog,int end)
{
/*	int x=(SCREEN_W-PROGRESS_WIDTH)/2;
	int y=SCREEN_H/2+1;
	int size=(PROGRESS_WIDTH-4)*prog/end;

	if(size)
	rectfill(screen,x+2,y+2,x+2+size-1,y+PROGRESS_HEIGHT-3,1);*/
}
