outf = open('data.txt', 'w')
prefix_length = 26+2*8+2*8+2*8+2*7+24

with open('data_raw.txt') as inf:
    for line in inf:
	if line[16] == '1':
            outf.write(line[prefix_length:])
