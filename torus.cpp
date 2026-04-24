#include "torus.h"



torus::torus():playfield() { }

void torus::prepare_lines(int detailmult)
{
	int torus_rings=detailmult*4;
	int torus_ringsegs=detailmult*4;

	alloc_arrays(torus_rings*torus_ringsegs,torus_rings*torus_ringsegs*2);

	int n;
	double a,sa,ca,b,sb,cb;

	n=0;
	for(int i=0;i<torus_rings;i++)
	{
		a=2*PI*double(i)/torus_rings;
		sa=sin(a);
		ca=cos(a);

		for(int j=0;j<torus_ringsegs;j++)
		{
			b=2*PI*double(j)/torus_ringsegs;
			sb=sin(b);
			cb=cos(b);

			vertices[n].pos.x=ca*majorrad+ca*cb*minorrad;
			vertices[n].pos.y=sa*majorrad+sa*cb*minorrad;
			vertices[n].pos.z=sb*minorrad;
			vertices[n].n.x=ca*cb;
			vertices[n].n.y=sa*cb;
			vertices[n].n.z=sb;
			n++;
		}
	}

	n=0;
	for(int i=0;i<torus_rings;i++)
	for(int j=0;j<torus_ringsegs;j++)
	{
		lines[n].a=i*torus_ringsegs+j;
		lines[n].b=i*torus_ringsegs+(j+1)%torus_ringsegs;
		n++;
		lines[n].a=i*torus_ringsegs+j;
		lines[n].b=((i+1)%torus_rings)*torus_ringsegs+j;
		n++;
	}

	playfield::prepare_lines(detailmult);
}

void torus::recalc_pos(vector &pos,vector &normal,vector &prev)
{
	double r,dx,dy;

	r=sqrt(pos.x*pos.x+pos.y*pos.y);
	dx=pos.x/r*majorrad;
	dy=pos.y/r*majorrad;

	pos.x-=dx; pos.y-=dy;

	normal=pos.normalized();

	pos=normal*minorrad;

	pos.x+=dx; pos.y+=dy;
}

void torus::random_pos(vector &pos,vector &normal,vector &prev)
{
	double a=2*PI*drand(),b;

	do { b=2*PI*drand(); }
	while(drand()>(majorrad+minorrad*cos(b))/majorrad/2/PI);
	// adjust for error in linear approximation

	pos.x=cos(a)*majorrad+cos(a)*cos(b)*minorrad;
	pos.y=sin(a)*majorrad+sin(a)*cos(b)*minorrad;
	pos.z=sin(b)*minorrad;
	normal.x=cos(a)*cos(b);
	normal.y=sin(a)*cos(b);
	normal.z=sin(b);
}



normtorus::normtorus():torus()
{
	name="Torus";
	ident="torus";
	minorrad=0.4;
	majorrad=0.6;
}

thicktorus::thicktorus():torus()
{
	name="Thick Torus";
	ident="thicktorus";
	minorrad=0.45;
	majorrad=0.55;
}

thintorus::thintorus():torus()
{
	name="Thin Torus";
	ident="thintorus";
	minorrad=0.3;
	majorrad=0.7;
}
