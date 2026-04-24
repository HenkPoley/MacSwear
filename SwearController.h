#import <Cocoa/Cocoa.h>

#import "XeeKeyboardShortcuts.h"


@class SwearView,SwearHighScoreView;

@interface SwearController:NSObject
{
	NSMenuItem *currlevelitem;

	IBOutlet SwearView *view;
	IBOutlet NSMenu *levelmenu;
	IBOutlet NSWindow *highscorewindow;
	IBOutlet SwearHighScoreView *highscoretable;
	IBOutlet NSPopUpButton *gamepopup;
	IBOutlet NSPopUpButton *levelpopup;
	IBOutlet XeeKeyboardShortcuts *shortcuts;
}

-(void)awakeFromNib;
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app;

-(void)buildLevelMenu;
-(void)selectLevel:(NSMenuItem *)item;

-(IBAction)startNormalGame:(id)sender;
-(IBAction)startHuntGame:(id)sender;
-(IBAction)startBlindGame:(id)sender;

-(void)buildLevelPopup;
-(IBAction)switchHighScore:(id)sender;

@end
