#import <Cocoa/Cocoa.h>

#ifdef SC_QT
# include <QtCollider.h>
#endif

int main(int argc, char *argv[])
{
#ifdef SC_QT
	[NSApplication sharedApplication];
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	return QtCollider::exec( argc, argv );
#else
	return NSApplicationMain(argc, argv);
#endif
}

