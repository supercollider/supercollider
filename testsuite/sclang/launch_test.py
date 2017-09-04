#!/usr/bin/env python
import fcntl
import sys
import os.path
import subprocess
import time
import select

# A quick-and-dirty launch test of sclang. This is temporary, but does the job for now.

def non_block_read(output):
    fd = output.fileno()
    fl = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)
    try:
        return output.readline()
    except:
        return ""

def sc_output_print(output):
	sys.stdout.write("\t|  " + output)

def sc_input(proc, input):
	print "\t|" + ("_" * 60)
	print "-> " + input
	print "\t_" + ("_" * 60)
	proc.stdin.write(input + (" %s" % chr(0x1b)))

timeout = 30
sclang_path = sys.argv[1]
assert os.path.exists(sclang_path)

env = dict(os.environ)

proc = subprocess.Popen([sclang_path, '-i' 'python'],
	stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE,
	env=env)

launched_string = "*** Welcome to SuperCollider"
start_time = time.time()
output = ""
while not(launched_string in output):
	if time.time() > (start_time + timeout):
		sys.exit("Timed out.")

	output = non_block_read(proc.stdout)
	error = non_block_read(proc.stderr)
	if error:
		print "ERROR:\n" + error
		sys.exit(error)
	elif output:
		sc_output_print(output)

	time.sleep(0.1)

# Quit sclang
sc_input(proc, "0.exit")

# Watch for a clean quit
output = ""
start_time = time.time()
while proc.poll() and time.time() < (start_time + timeout):
	if time.time() > (start_time + timeout):
		sys.exit("Timed out.")

	output = non_block_read(proc.stdout)
	error = non_block_read(proc.stderr)

	if error:
		# read the rest of the error
		print "ERROR:\n" + error
		sys.exit(error)
	elif output:
		sc_output_print(output)

	time.sleep(0.1)


sys.exit(0)
