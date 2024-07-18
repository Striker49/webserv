import sys

if (len(sys.argv) - 1) < 2:
	sys.exit(1)
f = open(sys.argv[1], "w")
f.write(sys.argv[2])
f.close()