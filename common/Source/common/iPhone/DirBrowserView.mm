//
//  DirBrowserView.m
//  iscsynth
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "DirBrowserView.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define TOOLBAR_HEIGHT		44

#define FILETRANSFER_PORT	15000

struct Header
{
	int bytes;
	int name_len;
};

@implementation FileTransferController

- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle browser:(DirBrowserView *)b
{
	if (self=[super initWithNibName:name bundle:bundle])
	{
		browser = b;
		service = [[NSNetService alloc] initWithDomain:@"local." type:@"_sctransfer._tcp" name:@"SuperCollider" port:FILETRANSFER_PORT];
		thread = 0;

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
/*
		unsigned long opt = fcntl(sock, F_GETFL);
		opt |= O_NONBLOCK;
		fcntl(sock, F_SETFL, opt); 
*/		
		int val = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)))
		{
			printf("couldn't set option\n");
			return 0;
		}
		
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(FILETRANSFER_PORT);
		
		if (bind(sock, (sockaddr *) &addr, sizeof(addr)))
		{
			printf("couldn't bind !\n");
			shutdown(sock, 0);
			sock = 0;
			return self;
			//return 0;
		}

		if (listen(sock, 1))
		{
			printf("couldn't listen\n");
			shutdown(sock, 0);
			sock = 0;
			return self;
			//return 0;
		}		
	}
	return self;
}

- (void) close:(id)sender
{
	[browser closeListen];
}

- (void) viewDidLoad
{
	[label setText:@"Waiting for connection..."];
	[progress setProgress:0.f];
	[self start];	
}

- (void) viewWillDisappear:(BOOL)animated
{
	[self stop];
}

- (void) start
{
	[service publish];
	
	if (thread) return;
	
	run_thread = 1;
	thread = [[NSThread alloc] initWithTarget:self selector:@selector(threadProc:) object:nil];
	[thread start];	
}

- (void) stop
{
	[service stop];
	//[thread stop];
	run_thread = 0;
	[thread release];
	thread = 0;
	shutdown(sock, 0);
}

- (void) threadProc:(id)arg
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	struct sockaddr_in their_addr;
	int len = sizeof(their_addr);
	
	while (run_thread)
	{
		usleep(1000);
		
		fd_set readfs;
		FD_ZERO(&readfs);
		FD_SET(sock, &readfs);
		struct timeval t;
		t.tv_sec = 0;
		t.tv_usec = 100000;
		if (select(sock+1, &readfs, 0, 0, &t)<=0) continue;
		
		int connection_sock = accept(sock, (sockaddr *) &their_addr, (socklen_t *) &len);
		if (connection_sock<0)
		{
			continue;
		}
		
		[self performSelectorOnMainThread:@selector(updateLabel:) withObject:@"Connected..." waitUntilDone:NO];
		
		Header h;
		int ret = recvfrom(connection_sock, &h, sizeof(h), 0, (sockaddr *) &their_addr, (socklen_t *) &len);
		if (ret!=sizeof(h))
		{
			printf("failed receiving header\n");
			shutdown(connection_sock, 0);
			continue;
		}
		
		int size = htonl(h.bytes);
		int name_len = htonl(h.name_len);
		
		if (!size || !name_len)
		{
			shutdown(connection_sock, 0);
			continue;
		}
		
		char *name = (char *) malloc(name_len+1);
		ret = recvfrom(connection_sock, name, name_len+1, 0, (sockaddr *) &their_addr, (socklen_t *) &len);
		if (ret!=name_len+1)
		{
			shutdown(connection_sock, 0);
			continue;
		}
		
		[self performSelectorOnMainThread:@selector(updateLabel:) withObject:[NSString stringWithFormat:@"Receiving %s...", name] waitUntilDone:NO];
		[self performSelectorOnMainThread:@selector(updateProgress:) withObject:[NSNumber numberWithFloat:0.f] waitUntilDone:NO];

		char *buf = (char *) malloc(size);
		int read = 0;
		
		while (read<size)
		{
			int ret = recvfrom(connection_sock, buf+read, size-read, 0, (sockaddr *) &their_addr, (socklen_t *) &len);
			if (ret<=0)
			{
				printf("failed receiving !");
				shutdown(connection_sock, 0);
				continue;
			}
			read += ret;
			[self performSelectorOnMainThread:@selector(updateProgress:) withObject:[NSNumber numberWithFloat:(float)(read/size)*0.5f] waitUntilDone:NO];
		}

		char output[512];
		output[0] = 0;
		
		NSString *path = [browser path];
		if (path)
		{
			strcpy(output, [path UTF8String]);
			strcat(output, "/");
		}
		strcat(output, name);
		
		FILE *f = fopen(output, "w");
		int written = 0;
		while (written<size)
		{
			int ret = fwrite(buf, 1, size-written, f);
			written += ret;
			buf += ret;
			[self performSelectorOnMainThread:@selector(updateProgress:) withObject:[NSNumber numberWithFloat:(float)(written/size)+0.5f] waitUntilDone:NO];
		}
		fclose(f);
		
		[self performSelectorOnMainThread:@selector(updateLabel:) withObject:[NSString stringWithFormat:@"Received %s", name] waitUntilDone:NO];
	}
	
	printf("bye bye !\n");
	
	[pool release];
}

- (void) updateLabel:(NSString *)string
{
	[label setText:string];
}

- (void) updateProgress:(NSNumber *)val
{
	[progress setProgress:[val floatValue]];
}

- (void) dealloc
{
	[service release];
	[super dealloc];
}


@end

@implementation DirBrowserViewController


- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle
{
    if (self = [super initWithNibName:name bundle:bundle]) {
        // Initialization code
		array = 0;
		target = 0;
		selector = 0;
		path = 0;
		root = 0;

		//controller = 0;
		
/*
		table = [[UITableView alloc] init];
		[self addSubview:table];
		[table setScrollEnabled:YES];
		[table setDelegate:self];
		[table setDataSource:self];
		[table setBounces:YES];
		
		toolbar = [[UIToolbar alloc] init];
		refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(triggerRefresh:)];
		[refreshItem setStyle:UIBarButtonItemStyleBordered];
		listenItem = [[UIBarButtonItem alloc] initWithTitle:@"Listen" style:UIBarButtonItemStyleBordered target:self action:@selector(triggerListen:)];
		[toolbar setItems:[NSArray arrayWithObjects:refreshItem,listenItem,nil]];
*/		
		fileController = [[FileTransferController alloc] initWithNibName:@"SuperCollider_FileTransfer" bundle:nil browser:self];
		
//		[self addSubview:toolbar];
    
//		[self layoutSubviews];
	}
    return self;
}

- (void) setController:(UIViewController *)c
{
	//controller = c;
}

- (void) layoutSubviews
{
/*
	CGRect contentRect = [self bounds];
	[table setFrame:CGRectMake(0,0,contentRect.size.width,contentRect.size.height-TOOLBAR_HEIGHT)];
	[toolbar setFrame:CGRectMake(0,contentRect.size.height-TOOLBAR_HEIGHT,contentRect.size.width,TOOLBAR_HEIGHT)];
*/
}

- (void) setPath:(NSString *)p
{
	if (path) [path release];
	path = [p retain];

	if (![self view]) return;

	[navItem setTitle:[path lastPathComponent]];
	[upItem setEnabled:[path compare:root]?YES:NO];
	
	[self refresh];
}

- (void) setRoot:(NSString *)p
{
	if (root) [root release];
	root = [p retain];
	
	[self setPath:p];	
}


- (void) viewDidLoad
{
	[table setDelegate:self];
	[table setDataSource:self];
	
	[self refresh];
}

- (NSString *) path
{
	return path;
}

- (void) setTarget:(id)t withSelector:(SEL)s
{
	target = t;
	selector = s;
}

- (void) refresh
{
	if (!path) return;
	if (array) [array release];
	array = [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:nil] retain];
		
	[table reloadData];
}

- (void) triggerRefresh:(id)sender
{
	[self refresh];
}

- (void) triggerUp:(id)sender
{
	if (![path compare:root]) return;
	NSString *p = [path stringByDeletingLastPathComponent];
	[self setPath:p];
}

- (void) triggerListen:(id)sender
{
	/*
	if (!controller) return;
	[controller presentModalViewController:fileController animated:YES];
	*/
	[self presentModalViewController:fileController animated:YES];
}

- (void) closeListen
{
	//[controller dismissModalViewControllerAnimated:YES];
	[self dismissModalViewControllerAnimated:YES];
	[self refresh];
}

- (void) flashScrollIndicators
{
	[table flashScrollIndicators];
}


- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return [array count];
}

- (UITableViewCell *) tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"MyCell"];
	if (!cell)
	{
		cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"MyCell"] autorelease];
	}
	NSString *item = (NSString *)[array objectAtIndex:indexPath.row];
	[cell setText:item];
	NSString *fullpath = [path stringByAppendingPathComponent:item];
	NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:fullpath error:nil];
	if ([attributes objectForKey:NSFileType]==NSFileTypeDirectory)
	{
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	}
	else cell.accessoryType = UITableViewCellAccessoryNone;
	return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)newIndexPath
{
	NSString *fullpath = [path stringByAppendingPathComponent:[array objectAtIndex:newIndexPath.row]];
	
	NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:fullpath error:nil];
	if ([attributes objectForKey:NSFileType]==NSFileTypeDirectory)
	{
		[tableView deselectRowAtIndexPath:newIndexPath animated:NO];
		[self setPath:fullpath];
		return;
	}
	
	[tableView deselectRowAtIndexPath:newIndexPath animated:YES];
	if (target && [target respondsToSelector:selector]) [target performSelector:selector withObject:fullpath];
}

- (void)drawRect:(CGRect)rect {
    // Drawing code
}


- (void)dealloc {
	if (path) [path release];
	
	/*
	[table release];
	[toolbar release];
	[refreshItem release];
	[listenItem release];
	*/
	[fileController release];
	
    [super dealloc];
}


@end
