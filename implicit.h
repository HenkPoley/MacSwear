#ifndef _IMPLICIT_H_
#define _IMPLICIT_H_

#include "playfield.h"

class implicit:public playfield
{
	public:
	implicit();

	void prepare_lines(int detailmult);
	void recalc_pos(vector &pos,vector &normal,vector &prev);
	void random_pos(vector &pos,vector &normal,vector &prev);

	virtual double func(const vector &r);

	vector del(const vector &r);
	vector findclosest(vector r);

	enum { BOX,SPHERE,SHELL,EXTSHELL } randmethod;
};

class dsphere:public implicit
{
	public:
	dsphere();
	double func(const vector &r);
};

class softcube:public implicit
{
	public:
	softcube();
	double func(const vector &r);
};

class metacage:public implicit
{
	public:
	metacage();
	double func(const vector &r);
};

class spikeball:public implicit
{
	public:
	spikeball();
	double func(const vector &r);
};

class fortress:public implicit
{
	public:
	fortress();
	double func(const vector &r);
};

class chair:public implicit
{
	public:
	chair();
	double func(const vector &r);
};

class sphericon:public implicit
{
	public:
	sphericon();
	double func(const vector &r);
};

class tritorus:public implicit
{
	public:
	tritorus();
	double func(const vector &r);
};

/*class trefoil:public implicit
{
	public:
	trefoil();
	double func(const vector &r);
};*/

class itest:public implicit
{
	public:
	itest();
	double func(const vector &r);
};



#define PILE_CHUNK 1024

class pointpile
{
	public:
	pointpile();
	~pointpile();

	int findpoint(vector_f p);

	vector_f *stuff;
	int size,n;
};

class linepile
{
	public:
	linepile();
	~linepile();

	line *getnew();

	line *stuff;
	int size,n;
};

#endif
