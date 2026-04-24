#import "SwearView.h"
#import "SwearHighScores.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <sys/time.h>

#import "game.h"
#import "playfield.h"
#import "sphere.h"
#import "torus.h"
#import "lathe.h"
#import "implicit.h"
#import "explicit.h"
#import "effects.h"


@implementation SwearView

-(id)initWithFrame:(NSRect)frameRect
{
	NSOpenGLPixelFormatAttribute attrs[]={ 
		NSOpenGLPFADoubleBuffer,
		(NSOpenGLPixelFormatAttribute)0};

	NSOpenGLPixelFormat *format=[[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];

	if(self=[super initWithFrame:frameRect pixelFormat:format])
	{
		pause=NO;
		level=nil;

		timer=nil;

		sg=new simplegame;
		hg=new huntgame; // fuooo~!
		bg=new blindgame;
		g=0;

		s=new starfield;

		[[self openGLContext] makeCurrentContext];

		long val=1;
		[[self openGLContext] setValues:&val forParameter:NSOpenGLCPSwapInterval];
	}

	return self;
}

-(void)dealloc
{
	delete sg;
	delete hg;
	delete bg;
	delete s;

	[self stopTimer];
	[level release];

	[super dealloc];
}

-(void)awakeFromNib
{
	[status setMessage:[NSString stringWithFormat:@"MacSwear %@ - Press a key to start",
	[[NSBundle bundleForClass:[self class]] objectForInfoDictionaryKey:@"CFBundleVersion"]]];
}

-(void)reshape
{
	[[self openGLContext] makeCurrentContext];
	[[self openGLContext] update];
}

-(void)drawRect:(NSRect)rect
{
	[[self openGLContext] makeCurrentContext];

	NSSize viewsize=[self bounds].size;
	glViewport(0,0,(int)viewsize.width,(int)viewsize.height);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if(g)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70,viewsize.width/viewsize.height,0.03,10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixd(g->cam_inv.a);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);

		if(s) s->draw(g->cam.position());
		g->draw();

		if([self isPaused])
		{
			glBlendFunc(GL_ZERO,GL_SRC_COLOR);
//			glDisable(GL_BLEND);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glColor3f(0.5,0.5,0.5);

			glBegin(GL_QUADS);
			glVertex2f(1,1);
			glVertex2f(1,-1);
			glVertex2f(-1,-1);
			glVertex2f(-1,1);
			glEnd();
		}
	}

//	glFlush();
	[[self openGLContext] flushBuffer];
}

-(void)keyDown:(NSEvent *)event
{
	[NSCursor setHiddenUntilMouseMoves:YES];

	XeeAction *action=[shortcuts actionForEvent:event];

	if(action)
	{
		NSString *ident=[action identifier];

		if([ident isEqual:@"speedUpKey"]) keys[KEY_UP]=true;
		else if([ident isEqual:@"quickTurnKey"]) keys[KEY_DOWN]=true;
		else if([ident isEqual:@"turnLeftKey"]) keys[KEY_LEFT]=true;
		else if([ident isEqual:@"turnRightKey"]) keys[KEY_RIGHT]=true;
		else if([ident isEqual:@"pauseKey"]) pause=!pause;
	}
	else
	{
		if(!g||g->over())
		{
			[self startGame:nil];
		}
	}
}

-(void)keyUp:(NSEvent *)event
{
	XeeAction *action=[shortcuts actionForEvent:event];
	if(!action) return;

	NSString *ident=[action identifier];

	if([ident isEqual:@"speedUpKey"]) keys[KEY_UP]=false;
	else if([ident isEqual:@"quickTurnKey"]) keys[KEY_DOWN]=false;
	else if([ident isEqual:@"turnLeftKey"]) keys[KEY_LEFT]=false;
	else if([ident isEqual:@"turnRightKey"]) keys[KEY_RIGHT]=false;
}

-(BOOL)acceptsFirstResponder { return YES; }



-(void)setLevel:(SwearLevel *)newlevel
{
	[level autorelease];
	level=[newlevel retain];
}

-(SwearLevel *)level { return level; }



-(void)startGame:(NSString *)gametype
{
	if(!level)
	{
		NSArray *levels=[SwearLevel levels];
		currentlevel=[levels objectAtIndex:grand()%[levels count]];
	}
	else currentlevel=level;

	playfield *p=(playfield *)[currentlevel playfield];

	if(gametype)
	{
		if([gametype isEqual:@"hunt"]) g=hg;
		else if([gametype isEqual:@"blind"]) g=bg;
		else g=sg;
	}
	else if(!g) g=sg;

	g->setup(p);

	[self prefsUpdated:nil]; // set up parameters and prepare field

	keys[KEY_LEFT]=false;
	keys[KEY_RIGHT]=false;
	keys[KEY_UP]=false;
	keys[KEY_DOWN]=false;

	prevtime=0;
	hasdied=NO;

	[status setHighScore:[highscores highScoreForLevel:[currentlevel identifier] game:[self game]]
	forName:[highscores highScoreNameForLevel:[currentlevel identifier] game:[self game]]];
	[status setMessage:nil];

	[self startTimer];
}



-(void)startTimer;
{
	if(!timer) timer=[[NSTimer scheduledTimerWithTimeInterval:1.0/60.0 target:self selector:@selector(runGame:) userInfo:nil repeats:YES] retain];
}

-(void)stopTimer
{
	[timer invalidate];
	[timer release];
	timer=nil;
}

-(void)runGame:(NSTimer *)timer
{
	struct timeval tv;
	gettimeofday(&tv,0);

	double time=(double)tv.tv_sec+((double)tv.tv_usec)/1000000.0;
	double dt;

	if(prevtime) dt=time-prevtime;
	else dt=0.01; // or whatever

	if(dt>0.1) dt=0.1;
//if(keys[KEY_DOWN]) dt/=10;
	prevtime=time;

	if(![self isPaused]) g->live(dt,keys);
	if(s) s->live(dt);

	[status setScore:g->score];
	[status setNext:g->next];

	if(g->over()&&!hasdied)
	{
		[highscores suggestHighScore:g->score time:(int)g->t
		level:[currentlevel identifier]
		game:[self game]];
		hasdied=YES;
	}

	[self setNeedsDisplay:YES];
}

-(NSString *)game
{
	if(g) return [NSString stringWithCString:g->ident encoding:NSISOLatin1StringEncoding];
	else return @"normal";
}

-(BOOL)isPaused
{
	if(pause) return YES;
	return ![[self window] isKeyWindow];
}

-(IBAction)prefsUpdated:(id)sender
{
	if(!g) return;

	g->camera_mode=[[NSUserDefaults standardUserDefaults] integerForKey:@"cameraMode"];
	g->camera_inertia=[[NSUserDefaults standardUserDefaults] integerForKey:@"cameraInertia"];
	g->tracking_angle=[[NSUserDefaults standardUserDefaults] integerForKey:@"trackingAngle"];

	int detail_level=[[NSUserDefaults standardUserDefaults] integerForKey:@"detailLevel"];
	int game_look=[[NSUserDefaults standardUserDefaults] integerForKey:@"gameLook"];

	g->p->prepare(detail_level,game_look);

	s->set_num([[NSUserDefaults standardUserDefaults] integerForKey:@"starField"],4);
	s->set_speed([[NSUserDefaults standardUserDefaults] integerForKey:@"starSpeed"],4);
}

@end



@implementation SwearStatusView

-(id)initWithFrame:(NSRect)frame
{
	if(self=[super initWithFrame:frame])
	{
		score=next=high=0;
		name=nil;
		message=nil;
	}
	return self;
}

-(void)dealloc
{
	[name release];
	[message release];
	[super dealloc];
}

-(void)drawRect:(NSRect)rect
{
	NSRect bounds=[self bounds];
	NSColor *green=[NSColor colorWithDeviceRed:0 green:0.66 blue:0 alpha:1];

	NSDictionary *attrs=[NSDictionary dictionaryWithObjectsAndKeys:
	[NSFont labelFontOfSize:bounds.size.height-6],NSFontAttributeName,
	green,NSForegroundColorAttributeName,
	nil];

	[[NSColor blackColor] set];
	[NSBezierPath fillRect:bounds];

	[green set];
	[NSBezierPath fillRect:NSMakeRect(0,bounds.size.height-2,bounds.size.width,1)];

	if(message)
	{
		NSSize msgsize=[message sizeWithAttributes:attrs];
		[message drawAtPoint:NSMakePoint((bounds.size.width-msgsize.width)/2,1) withAttributes:attrs];
	}
	else
	{
		NSString *scorestr=[NSString stringWithFormat:@"Score: %d  Next: %d",score,next];

		NSString *highstr;
		if(score>high) highstr=[NSString stringWithFormat:@"High score: %d by you!",score];
		else if(!name) highstr=[NSString stringWithFormat:@"High score: None"];
		else highstr=[NSString stringWithFormat:@"High score: %d by %@",high,name];

		NSSize highsize=[highstr sizeWithAttributes:attrs];

		[scorestr drawAtPoint:NSMakePoint(4,1) withAttributes:attrs];
		[highstr drawAtPoint:NSMakePoint(bounds.size.width-highsize.width-16,1) withAttributes:attrs];
	}
}

-(void)setScore:(int)newscore
{
	if(score!=newscore) [self setNeedsDisplay:YES];
	score=newscore;
}

-(void)setNext:(int)newnext
{
	if(next!=newnext) [self setNeedsDisplay:YES];
	next=newnext;
}

-(void)setHighScore:(int)newhigh forName:(NSString *)newname
{
	[self setNeedsDisplay:YES];
	high=newhigh;
	[name autorelease];
	name=[newname retain];
}

-(void)setMessage:(NSString *)newmessage
{
	[message autorelease];
	message=[newmessage retain];
}

@end



@implementation SwearLevel

-(id)initWithPlayfield:(void *)field
{
	if(self=[super init])
	{
		p=field;
	}
	return self;
}

-(void)dealloc
{
	delete (playfield *)p;
	[super dealloc];
}

-(NSString *)name
{
	return [NSString stringWithCString:((playfield *)p)->name encoding:NSISOLatin1StringEncoding];
}

-(NSString *)identifier
{
	return [NSString stringWithCString:((playfield *)p)->ident encoding:NSISOLatin1StringEncoding];
}

-(void *)playfield { return p; }

+(SwearLevel *)levelWithPlayfield:(void *)field
{
	return [[[SwearLevel alloc] initWithPlayfield:field] autorelease];
}

+(NSArray *)levels
{
	static NSArray *levels=nil;

	if(!levels)
	{
		levels=[[NSArray arrayWithObjects:

	//	[SwearLevel levelWithPlayfield:new etest()],
	//	[SwearLevel levelWithPlayfield:new itest()],

		[SwearLevel levelWithPlayfield:new sphere()],

		[SwearLevel levelWithPlayfield:new normtorus()],
		[SwearLevel levelWithPlayfield:new thicktorus()],
		[SwearLevel levelWithPlayfield:new thintorus()],

		[SwearLevel levelWithPlayfield:new softcube()],
		[SwearLevel levelWithPlayfield:new para()],
		[SwearLevel levelWithPlayfield:new sphericon()],

		[SwearLevel levelWithPlayfield:new barrel()],
		[SwearLevel levelWithPlayfield:new bread()],

		[SwearLevel levelWithPlayfield:new spool()],
		[SwearLevel levelWithPlayfield:new peanut()],
		[SwearLevel levelWithPlayfield:new ltop()],
		[SwearLevel levelWithPlayfield:new bowl()],

		[SwearLevel levelWithPlayfield:new moebius()],

		[SwearLevel levelWithPlayfield:new spikeball()],
		[SwearLevel levelWithPlayfield:new fortress()],
		[SwearLevel levelWithPlayfield:new metacage()],
		[SwearLevel levelWithPlayfield:new tritorus()],
		[SwearLevel levelWithPlayfield:new trefoil()],
		[SwearLevel levelWithPlayfield:new chair()],
		[SwearLevel levelWithPlayfield:new ufo()],

		[SwearLevel levelWithPlayfield:new mtwist()],
		[SwearLevel levelWithPlayfield:new klein()],
		[SwearLevel levelWithPlayfield:new spiral()],

		nil] retain];
	}

	return levels;
}

@end
