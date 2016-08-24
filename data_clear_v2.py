import sys

if len(sys.argv) != 2:
    print "[x] Expected input file"
    exit()

outf = open('data.txt', 'w')
prefix_length = 26+2*8+2*8+2*8+2*7+24

prefix_length = 113

with open(sys.argv[1]) as inf:
    for line in inf:
	if line[8] == 'I':
            #outf.write( ','.join(list(line[prefix_length:]))[:-1] )
	    outf.write( ','.join(list(line[prefix_length:].replace(' ', ''))[:-2]) + ',\n' )
	    #print ','.join(list(line[prefix_length:].replace(' ', ''))[:-1])

