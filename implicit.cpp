#include "implicit.h"



//#define IMPLICIT_DOTS 10240
//#define IMPLICIT_LINELENGTH 0.05

#define DEL_EPSILON 0.00000001 // 10 nano

#define SEARCH_STARTSIZE 0.01
#define SEARCH_NUMROUNDS 10

#define SGN(x) ((x)<0?(x)==0?0:-1:1)



implicit::implicit():playfield() { }

void implicit::prepare_lines(int detailmult)
{
	static const uint8 squares[16]={0,4,8,9,13,12,9+12*16,14,14,9+12*16,12,13,9,8,4,0};

	int implicit_grid=detailmult*2;

	double sqsize=2/double(implicit_grid-1);
	double u_offs[4]={sqsize/2,0,sqsize,sqsize/2};
	double v_offs[4]={0,sqsize/2,sqsize/2,sqsize};

	firstprogress();

	pointpile ppile;
	linepile lpile;

	for(int z=0;z<=implicit_grid;z++)
	{
		for(int x=0;x<implicit_grid;x++)
		for(int y=0;y<implicit_grid;y++)
		{
			double fx=(double(x)-0.5)*sqsize-1;
			double fy=(double(y)-0.5)*sqsize-1;
			double fz=(double(z)-0.5)*sqsize-1;
			int sq=0;

			if(func(vector(fx,fy,fz))<0) sq|=1;
			if(func(vector(fx+sqsize,fy,fz))<0) sq|=2;
			if(func(vector(fx,fy+sqsize,fz))<0) sq|=4;
			if(func(vector(fx+sqsize,fy+sqsize,fz))<0) sq|=8;

			sq=squares[sq];

			while(sq)
			{
				line *l=lpile.getnew();
				l->a=ppile.findpoint(vector_f(fx+u_offs[sq&3],fy+v_offs[sq&3],fz));
				l->b=ppile.findpoint(vector_f(fx+u_offs[(sq>>2)&3],fy+v_offs[(sq>>2)&3],fz));
				sq>>=4;
			}
		}
		showprogress(z,implicit_grid*4);
	}


	for(int y=0;y<=implicit_grid;y++)
	{
		for(int z=0;z<implicit_grid;z++)
		for(int x=0;x<implicit_grid;x++)
		{
			double fx=(double(x)-0.5)*sqsize-1;
			double fy=(double(y)-0.5)*sqsize-1;
			double fz=(double(z)-0.5)*sqsize-1;
			int sq=0;

			if(func(vector(fx,fy,fz))<0) sq|=1;
			if(func(vector(fx,fy,fz+sqsize))<0) sq|=2;
			if(func(vector(fx+sqsize,fy,fz))<0) sq|=4;
			if(func(vector(fx+sqsize,fy,fz+sqsize))<0) sq|=8;

			sq=squares[sq];

			while(sq)
			{
				line *l=lpile.getnew();
				l->a=ppile.findpoint(vector_f(fx+v_offs[sq&3],fy,fz+u_offs[sq&3]));
				l->b=ppile.findpoint(vector_f(fx+v_offs[(sq>>2)&3],fy,fz+u_offs[(sq>>2)&3]));
				sq>>=4;
			}
		}
		showprogress(y+implicit_grid,implicit_grid*4);
	}

	for(int x=0;x<=implicit_grid;x++)
	{
		for(int y=0;y<implicit_grid;y++)
		for(int z=0;z<implicit_grid;z++)
		{
			double fx=(double(x)-0.5)*sqsize-1;
			double fy=(double(y)-0.5)*sqsize-1;
			double fz=(double(z)-0.5)*sqsize-1;
			int sq=0;

			if(func(vector(fx,fy,fz))<0) sq|=1;
			if(func(vector(fx,fy+sqsize,fz))<0) sq|=2;
			if(func(vector(fx,fy,fz+sqsize))<0) sq|=4;
			if(func(vector(fx,fy+sqsize,fz+sqsize))<0) sq|=8;

			sq=squares[sq];

			while(sq)
			{
				line *l=lpile.getnew();
				l->a=ppile.findpoint(vector_f(fx,fy+u_offs[sq&3],fz+v_offs[sq&3]));
				l->b=ppile.findpoint(vector_f(fx,fy+u_offs[(sq>>2)&3],fz+v_offs[(sq>>2)&3]));
				sq>>=4;
			}
		}
		showprogress(x+2*implicit_grid,implicit_grid*4);
	}

	if(!ppile.n)
	{
/*		char *reason=(char *)(func(vector(0,0,0))<0?
		"Playfield seems completely filled.":
		"Playfield seems completely empty.");
		alert("No points generated!",reason,0,"Oops! My bad.",0,0,0);
		exit(666);*/
	}

	alloc_arrays(ppile.n,lpile.n);

	vector dummy;

	for(int i=0;i<num_vertices;i++)
	{
		vector pos=vector(ppile.stuff[i].x,ppile.stuff[i].y,ppile.stuff[i].z);
		vector normal;
		recalc_pos(pos,normal,dummy);
		vertices[i].pos=vector_f(pos.x,pos.y,pos.z);
		vertices[i].n=vector_f(normal.x,normal.y,normal.z);

		showprogress((i*implicit_grid)/num_vertices+3*implicit_grid,implicit_grid*4);
	}

	for(int i=0;i<num_lines;i++)
	{
		lines[i]=lpile.stuff[i];
	}

	showprogress(1,1);

	playfield::prepare_lines(detailmult);
}

void implicit::recalc_pos(vector &pos,vector &normal,vector &prev)
{
	pos=findclosest(pos);
	normal=del(pos).normalized();
}

void implicit::random_pos(vector &pos,vector &normal,vector &prev)
{
	double r;
	switch(randmethod)
	{
		case BOX:
			pos=vector::rand_cube();
		break;

		case SPHERE:
			pos=vector::rand_sphere();
		break;

		case SHELL:
			pos=vector::rand_sphere();
			r=pos.length();
			pos*=sqrt(r)/r;
		break;

		case EXTSHELL:
			pos=vector::rand_sphere();
			r=pos.length();
			pos*=sqrt(r)/r*1.15;
		break;
	}

	pos=findclosest(pos);

	normal=del(pos).normalized();
}



double implicit::func(const vector &r) { return(0); }



vector implicit::del(const vector &r)
{
//	return(r.normalized());
//	if(r.x>1) return(vector(1,0,0));
//	else if(r.x<-1) return(vector(-1,0,0));
//	else if(r.y>1) return(vector(0,1,0));
//	else if(r.y<-1) return(vector(0,-1,0));
//	else if(r.z>1) return(vector(0,0,1));
//	else if(r.z<-1) return(vector(0,0,-1));

	return(vector(
		(func(r+vector(DEL_EPSILON,0,0))-func(r-vector(DEL_EPSILON,0,0)))/2/DEL_EPSILON,
		(func(r+vector(0,DEL_EPSILON,0))-func(r-vector(0,DEL_EPSILON,0)))/2/DEL_EPSILON,
		(func(r+vector(0,0,DEL_EPSILON))-func(r-vector(0,0,DEL_EPSILON)))/2/DEL_EPSILON
	));
}

vector implicit::findclosest(vector r)
{
/*	double stepsize=SEARCH_STARTSIZE;
	for(int i=0;i<SEARCH_NUMROUNDS;i++)
	{
		int sgn=SGN(d_f(r));

		do { r-=del(r)*stepsize*sgn; }
		while(sgn==SGN(d_f(r)));

		stepsize/=SEARCH_SIZEDIV;
	}*/

	int sgn=SGN(func(r));
	vector dr;

	do { r-=dr=del(r).normalized()*SEARCH_STARTSIZE*sgn; }
	while(sgn==SGN(func(r)));

	sgn*=-1; // since we stepped over the boundary

	for(int i=0;i<SEARCH_NUMROUNDS;i++)
	{
		dr/=2;
		r+=dr;
		int newsgn=SGN(func(r));
		if(newsgn!=sgn) dr=-dr;
		sgn=newsgn;
	}

	return(r);
}



dsphere::dsphere():implicit() { name="implicit Sphere"; ident=0; }

double dsphere::func(const vector &r) { return(r.lengthsq()-1); }

softcube::softcube():implicit() { name="Soft Cube"; ident="softcube"; randmethod=BOX; }

double softcube::func(const vector &r)
{ return(r.x*r.x*r.x*r.x+r.y*r.y*r.y*r.y+r.z*r.z*r.z*r.z-0.75*0.75*0.75*0.75); }


metacage::metacage():implicit() { name="Metacage"; ident="metacage"; randmethod=BOX; }

#define LEVEL 1.05
#define HARDNESS 2
#define DIST 0.6

double metacage::func(const vector &r)
{
	return(10*(
//		-1/exp(HARDNESS*(r+vector(0,0,0.7)).lengthsq())
//		-1/exp(HARDNESS*(r-vector(0,0,0.7)).lengthsq())
		-exp(-HARDNESS*(r+vector(DIST,DIST,DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(DIST,DIST,-DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(DIST,-DIST,DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(DIST,-DIST,-DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(-DIST,DIST,DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(-DIST,DIST,-DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(-DIST,-DIST,DIST)).lengthsq())
		-exp(-HARDNESS*(r+vector(-DIST,-DIST,-DIST)).lengthsq())
		+LEVEL
	));
}

spikeball::spikeball():implicit() { name="Spike Ball"; ident="spikeball"; randmethod=SPHERE; }

double spikeball::func(const vector &r)
{
	return(
/*		-8/exp(8*(r.x*r.x+r.y*r.y))
		-8/exp(8*(r.y*r.y+r.z*r.z))
		-8/exp(8*(r.z*r.z+r.x*r.x))
		+5*(r.x*r.x+r.y*r.y+r.z*r.z-0.5)
		+3/exp((r.x*r.x+r.y*r.y+r.z*r.z))
		+4.5*/
		-8*exp(-8*(r.x*r.x+r.y*r.y))
		-8*exp(-8*(r.y*r.y+r.z*r.z))
		-8*exp(-8*(r.z*r.z+r.x*r.x))
		+5*(r.x*r.x+r.y*r.y+r.z*r.z-0.5)
		+4*exp(-(r.x*r.x+r.y*r.y+r.z*r.z))
		+4
	);
}

fortress::fortress():implicit() { name="Fortress"; ident="fortress"; randmethod=BOX; }

double fortress::func(const vector &r)
{
	return((
/*		-8/exp(8*((r.x-0.7)*(r.x-0.7)+r.y*r.y))
		-8/exp(8*((r.x+0.7)*(r.x+0.7)+r.y*r.y))
		-8/exp(8*(r.z*r.z+(r.y-0.7)*(r.y-0.7)))
		-8/exp(8*(r.z*r.z+(r.y+0.7)*(r.y+0.7)))
		+5*(r.x*r.x+r.y*r.y+r.z*r.z-0.7*0.7)
		+5*/
		-8*exp(-8*((r.x-0.58)*(r.x-0.58)+r.z*r.z))
		-8*exp(-8*((r.x+0.58)*(r.x+0.58)+r.z*r.z))
		-8*exp(-8*(r.z*r.z+(r.y-0.58)*(r.y-0.58)))
		-8*exp(-8*(r.z*r.z+(r.y+0.58)*(r.y+0.58)))
		-16*exp(-12*((r.x-0.58)*(r.x-0.58)+(r.y-0.58)*(r.y-0.58)))
		-16*exp(-12*((r.x+0.58)*(r.x+0.58)+(r.y-0.58)*(r.y-0.58)))
		-16*exp(-12*((r.x-0.58)*(r.x-0.58)+(r.y+0.58)*(r.y+0.58)))
		-16*exp(-12*((r.x+0.58)*(r.x+0.58)+(r.y+0.58)*(r.y+0.58)))
		+8*(r.x*r.x+r.y*r.y+r.z*r.z-0.6*0.6)
		+6)/10
	);
}

chair::chair():implicit() { name="Chair"; ident="chair"; randmethod=EXTSHELL; }

double chair::func(const vector &r)
{
	double a=0.95,b=0.8,k=5;
	double x=r.x*5,y=r.y*5,z=r.z*5;

	return(
		(x*x+y*y+z*z-a*k*k)*(x*x+y*y+z*z-a*k*k)
		-b*((z-k)*(z-k)-2*x*x)*((z+k)*(z+k)-2*y*y)
	);

}

sphericon::sphericon():implicit() { name="Sphericon"; ident="sphericon"; randmethod=SPHERE; }

double sphericon::func(const vector &r)
{
	return(r.x>0?
		(pow(r.x*r.x+r.y*r.y,1.05/2)+pow(fabs(r.z),1.05)-1):
		(pow(r.x*r.x+r.z*r.z,1.05/2)+pow(fabs(r.y),1.05)-1)
	);
}

tritorus::tritorus():implicit() { name="Tritorus"; ident="tritorus"; randmethod=SHELL; }

double tritorus::func(const vector &r)
{
	double x=r.x/0.95,y=r.y/0.95,z=r.z/0.95;

	return(
		-exp(-32*(x*x+y*y-2*0.8*sqrt(x*x+y*y)+0.8*0.8+z*z))
		-exp(-32*(y*y+z*z-2*0.8*sqrt(y*y+z*z)+0.8*0.8+x*x))
		-exp(-32*(z*z+x*x-2*0.8*sqrt(z*z+x*x)+0.8*0.8+y*y))
		+0.22
	);
}



/*trefoil::trefoil():implicit() { name="Trefoil knot"; ident="trefoil"; randmethod=SPHERE; }

double trefoil::func(const vector &r)
{
	double majorrad=0.55;
	double minorrad=0.25;
	double thickness=0.2;

	double dist=sqrt(r.x*r.x+r.y*r.y)-majorrad;
	double polar=atan2(r.y,r.x);
	double ringwise=atan2(dist,r.z);

//	double rad=sqrt(dist*dist+r.z*r.z);

	double dr=minorrad*sin(polar*1.5);
	double dz=minorrad*cos(polar*1.5);

	double rad1=sqrt((dist-dr)*(dist-dr)+(r.z-dz)*(r.z-dz));
	double rad2=sqrt((dist+dr)*(dist+dr)+(r.z+dz)*(r.z+dz));

	return(
		MIN(rad1,rad2)-thickness
	);
	
}*/


// http://www.uib.no/People/nfytn/surfaces.htm

itest::itest():implicit() { name="Implicit Test"; ident=0; randmethod=BOX; }

double itest::func(const vector &r)
{
	double x=r.x,y=r.y,z=r.z;

	return(
		x*x+y*y+z*z-1
		+0.01/(x*x+y*y)
		+0.01/(y*y+z*z)
		+0.01/(z*z+x*x)
	
	);

	// spikecube
/*	double sc=3;
	double x=r.x*sc,y=r.y*sc,z=r.z*sc;

	return(
		x*x*x*x+y*y*y*y+z*z*z*z-x*x-y*y-z*z-x*x*y*y-x*x*z*z-y*y*z*z+1
		-0.4*(x*x+y*y+z*z)
		+0.07*(x*x+y*y+z*z)*(x*x+y*y+z*z)
	);*/

	// droplet
//	return(0.5*r.x*r.x*r.x*r.x*r.x + 0.5*r.x*r.x*r.x*r.x - (r.y*r.y + r.z*r.z));

	// deformed torus
/*	double x=r.x*3,y=r.y*3,z=r.z*3;

	y-=1.7;

	double r2=x*x+y*y+z*z;

	x/=r2;
	y/=r2;
	z/=r2;

	y+=1/1.7;

	return(-(
		x*x+y*y-2*0.6*sqrt(x*x+y*y)+0.6*0.6
		+z*z
		-0.4*0.4
	));*/

	// cathedral
/*	return((
		-8/exp(8*((r.x-0.7)*(r.x-0.7)+r.y*r.y))
		-8/exp(8*((r.x+0.7)*(r.x+0.7)+r.y*r.y))
		-8/exp(8*(r.z*r.z+(r.y-0.7)*(r.y-0.7)))
		-8/exp(8*(r.z*r.z+(r.y+0.7)*(r.y+0.7)))
		+5*(r.x*r.x+r.y*r.y+r.z*r.z-0.7*0.7)
		+5)/10
	);*/

	// goursat's *
/*	double x=r.x*2,y=r.y*2,z=r.z*2;

	return(x*x*x*x+y*y*y*y+z*z*z*z-2*(x*x+y*y+z*z)-1);*/

	// heart ?
/*	double x=r.x*1.2,y=r.y*1.2,z=r.z*1.2;

	return(
		(2*x*x+y*y+z*z-1)*(2*x*x+y*y+z*z-1)*(2*x*x+y*y+z*z-1)
		-x*x*z*z*z/10-y*y*z*z*z
	);*/

	// superegg *
/*	double x=r.x,y=r.y,z=r.z;
	double a=1,b=1,n=3;

	return(pow(sqrt(x*x+y*y)/a,n)+pow(z/b,n)-1);*/

	// tanglecube
/*	double x=r.x*3,y=r.y*3,z=r.z*3;

	return(x*x*x*x-5*x*x+y*y*y*y-5*y*y+z*z*z*z-5*z*z+10);*/

	// cushion *
/*	double x=r.x*6,y=r.y*6,z=r.z*6;

	return(z*z*x*x-z*z*z*z-2*z*x*x+2*z*z*z+x*x-z*z);*/
}




pointpile::pointpile()
{
	size=PILE_CHUNK;
	n=0;
	stuff=new vector_f[size];
}

pointpile::~pointpile()
{
	delete stuff;
}

int pointpile::findpoint(vector_f p)
{
	for(int i=0;i<n;i++)
	if(p==stuff[i]) return(i);

	if(n==size) // should only ever be ==
	{
		vector_f *newstuff;
		size+=PILE_CHUNK;
		newstuff=new vector_f[size];

		for(int i=0;i<n;i++) newstuff[i]=stuff[i];

		delete stuff;
		stuff=newstuff;
	}

	stuff[n]=p;

	return(n++);
}

linepile::linepile()
{
	size=PILE_CHUNK;
	n=0;
	stuff=new line[size];
}

linepile::~linepile()
{
	delete stuff;
}

line *linepile::getnew()
{
	if(n==size) // should only ever be ==
	{
		line *newstuff;
		size+=PILE_CHUNK;
		newstuff=new line[size];

		for(int i=0;i<n;i++) newstuff[i]=stuff[i];

		delete stuff;
		stuff=newstuff;
	}

	return(&stuff[n++]);
}
