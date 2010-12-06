//
//  FileBrowserViewController.m
//  iscsynth
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "FileBrowserViewController.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

/*
#define FILETRANSFER_PORT	15000

struct Header
{
	int bytes;
	int name_len;
};

@implementation FileTransferController

- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle browser:(FileBrowserViewController *)b
{
	if (self=[super initWithNibName:name bundle:bundle])
	{
		browser = b;
		service = [[NSNetService alloc] initWithDomain:@"local." type:@"_sctransfer._tcp" name:@"SuperCollider" port:FILETRANSFER_PORT];
		thread = 0;

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
		}

		if (listen(sock, 1))
		{
			printf("couldn't listen\n");
			shutdown(sock, 0);
			sock = 0;
			return self;
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

*/

@implementation FileBrowserViewController

- (id)initWithCoder:(NSCoder *)decoder
{
    if (self = [super initWithCoder:decoder])
	{
		target = 0;
		selector = 0;
		[self setup];
	}
    return self;
}

- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle
{
    if (self = [super initWithNibName:name bundle:bundle])
	{
		target = 0;
		selector = 0;
		[self setup];
	}
    return self;
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

- (void) setup
{
	FileBrowserPageViewController *m = [[FileBrowserPageViewController alloc] initWithNibName:@"SuperCollider_BrowserPage" bundle:nil];
	[self pushViewController:m animated:NO];
	[m release];
}

- (void) setPath:(NSString *)p
{
	FileBrowserPageViewController *m = (FileBrowserPageViewController *) [self.viewControllers objectAtIndex:0];
	if (m)
	{
		[m setPath:p];
		m.title = @"Documents";
	}
}

- (void) setTarget:(id)t withSelector:(SEL)s
{
	target = [t retain];
	selector = s;
}

- (void) didSelect:(NSString *) path
{
	if (target && [target respondsToSelector:selector]) [target performSelector:selector withObject:path];
}

- (void) dealloc
{
	if (target) [target release];
	[super dealloc];
}

@end

@implementation FileBrowserPageViewController

- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle
{
    if (self = [super initWithNibName:name bundle:bundle])
	{
		array = 0;
		path = 0;
	}
    return self;
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

- (void) setPath:(NSString *)p
{
	if (path) [path release];
	path = [p retain];

	self.title = [path lastPathComponent];
	
	[self refresh];
}

- (void) viewDidLoad
{
	self.navigationItem.rightBarButtonItem = refreshButton;
	
	[self refresh];
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
		FileBrowserPageViewController *c = [[FileBrowserPageViewController alloc] initWithNibName:@"SuperCollider_BrowserPage" bundle:nil];
		[c setPath:fullpath];
		[self.navigationController pushViewController:c animated:YES];
		[c release];
		return;
	}
	
	[tableView deselectRowAtIndexPath:newIndexPath animated:YES];
	[(FileBrowserViewController *) self.navigationController didSelect:fullpath];
}

- (void)dealloc
{
	if (path) [path release];
	if (array) [array release];
	
    [super dealloc];
}


@end
