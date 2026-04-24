#import <Cocoa/Cocoa.h>


@class SwearHighScoreView;

@interface SwearHighScores:NSObject
{
	NSMutableDictionary *levels;
	int numscores;

	IBOutlet NSWindow *gamewindow;
	IBOutlet NSWindow *highscorewindow;
	IBOutlet SwearHighScoreView *highscoretable;
}

-(id)init;
-(void)dealloc;
-(void)awakeFromNib;

-(void)load;
-(void)save;

-(int)highScoreForLevel:(NSString *)level game:(NSString *)game;
-(NSString *)highScoreNameForLevel:(NSString *)level game:(NSString *)game;

-(void)suggestHighScore:(int)score time:(int)time level:(NSString *)level game:(NSString *)game;
-(NSMutableArray *)getScoreArrayForLevel:(NSString *)level game:(NSString *)game;
-(void)setScoreArray:(NSArray *)scores forLevel:(NSString *)level game:(NSString *)game;

-(void)openHighScoreSheetForEntry:(int)n level:(NSString *)level game:(NSString *)game;
-(void)endHighScoreView;

-(id)tableView:(SwearHighScoreView *)table objectValueForTableColumn:(NSTableColumn *)column row:(int)row;
-(void)tableView:(SwearHighScoreView *)table setObjectValue:(id)obj forTableColumn:(NSTableColumn *)column row:(int)row;
-(int)numberOfRowsInTableView:(SwearHighScoreView *)table;

-(BOOL)tableView:(SwearHighScoreView *)table shouldEditTableColumn:(NSTableColumn *)column row:(int)row;

@end



@interface SwearHighScoreView:NSTableView
{
	NSString *game;
	NSString *level;
	int editline;
}

-(id)initWithCoder:(NSCoder *)coder;
-(void)dealloc;

-(void)setGame:(NSString *)newgame;
-(void)setLevel:(NSString *)newlevel;
-(NSString *)game;
-(NSString *)level;

-(void)editLine:(int)n;
-(int)editingLine;

-(void)textDidEndEditing:(NSNotification *)notification;

@end
