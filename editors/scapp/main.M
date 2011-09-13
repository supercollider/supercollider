#import <Cocoa/Cocoa.h>

#ifdef SC_QT
# include <QtCollider.h>
#endif

#ifdef SC_QT

int main(int argc, char *argv[])
{

	[NSApplication sharedApplication];
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	return QtCollider::exec( argc, argv );
}

#else

int main(int argc, const char *argv[])
{
	return NSApplicationMain(argc, argv);
}

#endif
