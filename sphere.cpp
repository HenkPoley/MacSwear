#include "sphere.h"

#include <math.h>



sphere::sphere():playfield() { name="Sphere"; ident="sphere"; }

void sphere::prepare_lines(int detailmult)
{
	int sphere_rings=detailmult*4-1;
	int sphere_ringsegs=detailmult*4;

	alloc_arrays(sphere_rings*sphere_ringsegs+2,sphere_rings*sphere_ringsegs+(sphere_rings+1)*sphere_ringsegs);

	int n;
	double a,sa,ca,b;

	n=0;
	for(int i=0;i<sphere_rings;i++)
	{
		a=PI*double(i+1)/(sphere_rings+1)-PI;
		sa=sin(a);
		ca=cos(a);

		for(int j=0;j<sphere_ringsegs;j++)
		{
			b=2*PI*double(j)/sphere_ringsegs;

			vertices[n].pos=vertices[n].n=vector_f(sa*cos(b),sa*sin(b),ca);
			n++;
		}
	}

	vertices[n].pos=vertices[n].n=vector_f(0,0,1);
	n++;

	vertices[n].pos=vertices[n].n=vector_f(0,0,-1);
	n++;

	n=0;
	for(int i=0;i<sphere_rings;i++)
	for(int j=0;j<sphere_ringsegs;j++)
	{
		lines[n].a=i*sphere_ringsegs+j;
		lines[n].b=i*sphere_ringsegs+(j+1)%sphere_ringsegs;
		n++;
	}

	for(int i=0;i<sphere_rings-1;i++)
	for(int j=0;j<sphere_ringsegs;j++)
	{
		lines[n].a=i*sphere_ringsegs+j;
		lines[n].b=(i+1)*sphere_ringsegs+j;
		n++;
	}

	for(int j=0;j<sphere_ringsegs;j++)
	{
		lines[n].a=j;
		lines[n].b=num_vertices-1;
		n++;

		lines[n].a=j+(sphere_rings-1)*sphere_ringsegs;
		lines[n].b=num_vertices-2;
		n++;
	}

	playfield::prepare_lines(detailmult);
}

void sphere::recalc_pos(vector &pos,vector &normal,vector &prev)
{
	pos.normalize();
	normal=pos;
}

void sphere::random_pos(vector &pos,vector &normal,vector &prev)
{
	pos=vector::rand_sphere().normalized();
	normal=pos;
}
