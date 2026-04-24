#import "SwearHighScores.h"




@implementation SwearHighScores

-(id)init
{
	if(self=[super init])
	{
		numscores=10;

		levels=[[NSMutableDictionary dictionary] retain];
		highscoretable=nil;

		[self load];
	}
	return self;
}

-(void)dealloc
{
	[levels release];
	//[highscoretable release]; // I dunno, should I? doesn't really matter I guess.

	[super dealloc];
}

-(void)awakeFromNib
{
}

-(void)load
{
	NSDictionary *scores=[[NSUserDefaults standardUserDefaults] objectForKey:@"highScores"];
	if(scores)
	{
		[levels removeAllObjects];
		[levels addEntriesFromDictionary:scores];
	}
}

-(void)save
{
	[[NSUserDefaults standardUserDefaults] setObject:levels forKey:@"highScores"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

-(int)highScoreForLevel:(NSString *)level game:(NSString *)game
{
	NSMutableArray *scores=[self getScoreArrayForLevel:level game:game];
	if([scores count]) return [[[scores objectAtIndex:0] objectForKey:@"score"] intValue];
	else return 0;
}

-(NSString *)highScoreNameForLevel:(NSString *)level game:(NSString *)game;
{
	NSMutableArray *scores=[self getScoreArrayForLevel:level game:game];
	if([scores count]) return [[scores objectAtIndex:0] objectForKey:@"name"];
	else return nil;
}

-(void)suggestHighScore:(int)score time:(int)time level:(NSString *)level game:(NSString *)game
{
	NSMutableArray *scores=[self getScoreArrayForLevel:level game:game];

	if([scores count]>=numscores)
	{
		NSDictionary *lowest=[scores lastObject];
		if([[lowest objectForKey:@"score"] intValue]>=score) return;
	}

	NSString *name=[[NSUserDefaults standardUserDefaults] objectForKey:@"defaultName"];
	if(!name) name=@"";

	NSDictionary *dict=[NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithInt:score],@"score",
		[NSNumber numberWithInt:time],@"time",
		[NSNumber numberWithInt:(int)[[NSDate date] timeIntervalSince1970]],@"date",
		name,@"name",
	nil];

	int n=0;
	while(n<[scores count]&&[[[scores objectAtIndex:n] objectForKey:@"score"] intValue]>=score) n++;

	[scores insertObject:dict atIndex:n];

	while([scores count]>numscores) [scores removeLastObject];

	[self setScoreArray:scores forLevel:level game:game];

	[self openHighScoreSheetForEntry:n level:level game:game];
}

-(NSMutableArray *)getScoreArrayForLevel:(NSString *)level game:(NSString *)game
{
	NSDictionary *games=[levels objectForKey:level];
	if(!games) return [NSMutableArray array];

	NSArray *scores=[games objectForKey:game];
	if(!scores) return [NSMutableArray array];

	return [NSMutableArray arrayWithArray:scores];
}

-(void)setScoreArray:(NSArray *)scores forLevel:(NSString *)level game:(NSString *)game
{
	NSDictionary *games=[levels objectForKey:level];
	NSArray *array=[NSArray arrayWithArray:scores];

	if(games)
	{
		NSMutableDictionary *mute=[NSMutableDictionary dictionaryWithDictionary:games];
		[mute setObject:array forKey:game];
		[levels setObject:[NSDictionary dictionaryWithDictionary:mute] forKey:level];
	}
	else
	{
		[levels setObject:[NSDictionary dictionaryWithObject:array forKey:game] forKey:level];
	}
}

-(void)openHighScoreSheetForEntry:(int)n level:(NSString *)level game:(NSString *)game
{
	[highscoretable setLevel:level];
	[highscoretable setGame:game];
	[highscoretable reloadData];
	[highscoretable editLine:n];
	[[NSApplication sharedApplication] beginSheet:highscorewindow modalForWindow:gamewindow modalDelegate:nil didEndSelector:nil contextInfo:nil];
}

-(void)endHighScoreView
{
	[[NSApplication sharedApplication] endSheet:highscorewindow];
	[highscorewindow orderOut:nil];
}

-(id)tableView:(SwearHighScoreView *)table objectValueForTableColumn:(NSTableColumn *)column row:(int)row
{
	NSArray *scores=[self getScoreArrayForLevel:[table level] game:[table game]];
	NSString *identifier=[column identifier];

	if([identifier isEqual:@"rank"]) return [NSNumber numberWithInt:row+1];
	else return [[scores objectAtIndex:row] objectForKey:[column identifier]];
}

-(void)tableView:(SwearHighScoreView *)table setObjectValue:(id)obj forTableColumn:(NSTableColumn *)column row:(int)row
{
	NSString *identifier=[column identifier];
	if(![identifier isEqual:@"name"]) return;
	if([table editingLine]!=row) return;

	[[NSUserDefaults standardUserDefaults] setObject:obj forKey:@"defaultName"];

	NSMutableArray *scores=[self getScoreArrayForLevel:[table level] game:[table game]];

	if(!obj||![obj length]) obj=@"Mr. Return";

	NSMutableDictionary *dict=[NSMutableDictionary dictionaryWithDictionary:[scores objectAtIndex:row]];
	[dict setObject:obj forKey:@"name"];
	[scores replaceObjectAtIndex:row withObject:[NSDictionary dictionaryWithDictionary:dict]];
	[self setScoreArray:scores forLevel:[table level] game:[table game]];

	[self save];
}

-(int)numberOfRowsInTableView:(SwearHighScoreView *)table
{
	return [[self getScoreArrayForLevel:[table level] game:[table game]] count];
}

-(BOOL)tableView:(SwearHighScoreView *)table shouldEditTableColumn:(NSTableColumn *)column row:(int)row
{
	return NO;
}

@end



@implementation SwearHighScoreView

-(id)initWithCoder:(NSCoder *)coder
{
	if(self=[super initWithCoder:coder])
	{
		game=nil;
		level=nil;
		editline=-1;
	}
	return self;
}

-(void)dealloc
{
	[game release];
	[level release];

	[super dealloc];
}

-(void)setGame:(NSString *)newgame
{
	[game autorelease];
	game=[newgame retain];
}

-(void)setLevel:(NSString *)newlevel
{
	[level autorelease];
	level=[newlevel retain];
}

-(NSString *)game { return game; }

-(NSString *)level { return level; }



-(void)editLine:(int)n
{
	editline=n;
	[self selectRowIndexes:[NSIndexSet indexSetWithIndex:n] byExtendingSelection:NO];
	[self editColumn:1 row:n withEvent:nil select:YES];
}

-(int)editingLine { return editline; }

-(void)textDidEndEditing:(NSNotification *)notification
{
    NSDictionary *info=[notification userInfo];
//	int movement=[[info valueForKey:@"NSTextMovement"] intValue];

	NSMutableDictionary *newinfo=[NSMutableDictionary dictionaryWithDictionary:info];
	[newinfo setObject:[NSNumber numberWithInt:NSIllegalTextMovement] forKey:@"NSTextMovement"];

	[super textDidEndEditing:[NSNotification notificationWithName:[notification name]
	object:[notification object] userInfo:newinfo]];

	[(SwearHighScores *)[self dataSource] endHighScoreView];
	editline=-1;
}

@end
