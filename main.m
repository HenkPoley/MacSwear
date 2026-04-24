#import <Cocoa/Cocoa.h>

int main(int argc,char *argv[])
{
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
	[[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
		@"2",@"cameraMode",
		@"0",@"cameraInertia",
		@"0",@"trackingAngle",
		@"1",@"detailLevel",
		@"0",@"gameLook",
		@"0",@"starField",
		@"0",@"starSpeed",
	nil]];
    [pool release];

    return NSApplicationMain(argc,(const char **) argv);
}
