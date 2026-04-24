#import "SwearController.h"
#import "SwearView.h"
#import "SwearHighScores.h"
#import "random.h"

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <time.h>



@implementation SwearController

-(void)awakeFromNib
{
	currlevelitem=nil;

	[self buildLevelMenu];
	[self buildLevelPopup];
	[self switchHighScore:nil];

	[shortcuts addActions:[NSArray arrayWithObjects:
		[XeeAction actionWithTitle:@"Turn left" identifier:@"turnLeftKey"],
		[XeeAction actionWithTitle:@"Turn right" identifier:@"turnRightKey"],
		[XeeAction actionWithTitle:@"Speed up" identifier:@"speedUpKey"],
		[XeeAction actionWithTitle:@"Quick turn" identifier:@"quickTurnKey"],
		[XeeAction actionWithTitle:@"Pause" identifier:@"pauseKey"],
//		[XeeAction actionWithTitle:@"Restart" identifier:@"restartKey"],
	0]];
	[shortcuts addShortcuts:[NSDictionary dictionaryWithObjectsAndKeys:
/*		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:27 modifiers:0],
		0],@"closeWindowOrDrawer:",*/
		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:NSLeftArrowFunctionKey modifiers:0],
			[XeeKeyStroke keyForCharacter:@"a" modifiers:0],
		0],@"turnLeftKey",
		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:NSRightArrowFunctionKey modifiers:0],
			[XeeKeyStroke keyForCharacter:@"d" modifiers:0],
		0],@"turnRightKey",
		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:NSUpArrowFunctionKey modifiers:0],
			[XeeKeyStroke keyForCharacter:@"w" modifiers:0],
		0],@"speedUpKey",
		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:NSDownArrowFunctionKey modifiers:0],
			[XeeKeyStroke keyForCharacter:@"s" modifiers:0],
		0],@"quickTurnKey",
		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharacter:@"p" modifiers:0],
		0],@"pauseKey",
/*		[NSArray arrayWithObjects:
			[XeeKeyStroke keyForCharCode:13 modifiers:0],
			[XeeKeyStroke keyForCharCode:32 modifiers:0],
		0],@"restartKey",*/
	0]];

	sgrand(time(NULL));
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}

-(void)buildLevelMenu
{
	NSString *deflevelname=[[NSUserDefaults standardUserDefaults] stringForKey:@"selectedLevel"];
	NSMenuItem *defitem=nil;

	NSEnumerator *enumerator=[[SwearLevel levels] objectEnumerator];
	SwearLevel *level;

	while(level=[enumerator nextObject])
	{
		NSMenuItem *item=[[[NSMenuItem alloc] initWithTitle:[level name] action:@selector(selectLevel:) keyEquivalent:@""] autorelease];
		//[item setImage:image];
		[item setRepresentedObject:level];
		[levelmenu addItem:item];

		if([[level name] isEqual:deflevelname]) defitem=item;
	}
	[levelmenu addItem:[NSMenuItem separatorItem]];

	NSMenuItem *randitem=[[[NSMenuItem alloc] initWithTitle:@"Random" action:@selector(selectLevel:) keyEquivalent:@""] autorelease];
	[randitem setRepresentedObject:nil];
	[levelmenu addItem:randitem];

	if(!defitem)
	{
		if([deflevelname isEqual:@"Random"]) defitem=randitem;
		else defitem=[levelmenu itemAtIndex:0];
	}

	[defitem setState:NSOnState];
	[view setLevel:[defitem representedObject]];
	currlevelitem=defitem;
}

-(void)selectLevel:(NSMenuItem *)item
{
	SwearLevel *level=[item representedObject];

	[currlevelitem setState:NSOffState];
	[item setState:NSOnState];
	currlevelitem=item;

	NSString *name;
	if(level) name=[level name];
	else name=@"Random";
	[[NSUserDefaults standardUserDefaults] setObject:name forKey:@"selectedLevel"];

	[view setLevel:level];
	[view startGame:nil];
}

-(IBAction)startNormalGame:(id)sender
{
	[view startGame:@"simple"];
}

-(IBAction)startHuntGame:(id)sender
{
	[view startGame:@"hunt"];
}

-(IBAction)startBlindGame:(id)sender
{
	[view startGame:@"blind"];
}



-(void)buildLevelPopup
{
	NSEnumerator *enumerator=[[SwearLevel levels] objectEnumerator];
	SwearLevel *level;

	[levelpopup removeAllItems];

	while(level=[enumerator nextObject])
	{
		NSMenuItem *item=[[[NSMenuItem alloc] initWithTitle:[level name] action:@selector(switchHighScore:) keyEquivalent:@""] autorelease];
		[item setRepresentedObject:level];
		[[levelpopup menu] addItem:item];
	}
}

-(IBAction)switchHighScore:(id)sender
{
	switch([gamepopup indexOfSelectedItem])
	{
		case 0: [highscoretable setGame:@"normal"]; break;
		case 1: [highscoretable setGame:@"hunt"]; break;
		case 2: [highscoretable setGame:@"blind"]; break;
	}

	[highscoretable setLevel:[[[levelpopup selectedItem] representedObject] identifier]];
	[highscoretable reloadData];
}

@end
