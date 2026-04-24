#import <Cocoa/Cocoa.h>

#import "XeeKeyboardShortcuts.h"

@class SwearLevel,SwearStatusView,SwearHighScores;

class game;
class simplegame;
class huntgame;
class blindgame;
class starfield;

@interface SwearView:NSOpenGLView
{
	BOOL pause,hasdied;
	SwearLevel *level,*currentlevel;

	NSTimer *timer;
	game *g;
	starfield *s;

	simplegame *sg;
	huntgame *hg;
	blindgame *bg;
//	SwearLevel *level;
	bool keys[4];

	double prevtime;

	IBOutlet XeeKeyboardShortcuts *shortcuts;
	IBOutlet SwearStatusView *status;
	IBOutlet SwearHighScores *highscores;
}

-(id)initWithFrame:(NSRect)frame;
-(void)dealloc;
-(void)awakeFromNib;

-(void)reshape;
-(void)drawRect:(NSRect)rect;

-(void)keyDown:(NSEvent *)event;
-(void)keyUp:(NSEvent *)event;
-(BOOL)acceptsFirstResponder;

-(void)setLevel:(SwearLevel *)newlevel;
-(SwearLevel *)level;

-(void)startGame:(NSString *)gametype;

-(void)startTimer;
-(void)stopTimer;

-(void)runGame:(NSTimer *)timer;
-(NSString *)game;
-(BOOL)isPaused;

-(IBAction)prefsUpdated:(id)sender;

@end



@interface SwearStatusView:NSView
{
	int score,next,high;
	NSString *name,*message;
}

-(id)initWithFrame:(NSRect)frame;
-(void)dealloc;

-(void)drawRect:(NSRect)rect;

-(void)setScore:(int)newscore;
-(void)setNext:(int)newnext;
-(void)setHighScore:(int)newhigh forName:(NSString *)newname;
-(void)setMessage:(NSString *)newmessage;

@end



@interface SwearLevel:NSObject
{
	void *p;
}

-(id)initWithPlayfield:(void *)field;
-(void)dealloc;

-(NSString *)name;
-(NSString *)identifier;
-(void *)playfield;

+(SwearLevel *)levelWithPlayfield:(void *)field;
+(NSArray *)levels;

@end
