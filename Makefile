
#g++ -g -Wall -o bin/main src/main.cpp

# the compiler: gcc for C program, define as g++ for C++
CC = gcc -std=gnu99

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -g
CLIBDIR = -L /usr/local/ssl/lib 
CHEADER_DIR = -I src/include/ -I /usr/local/ssl/include/ -I openssl_aes_ref_impl/
CLIBS   = -lm 

# the build target executable:
TARGET = main






rev: src/rev.c src/cssarev.c
	$(CC) $(CFLAGS) -o bin/rev src/rev.c src/cssarev.c -O0 $(CHEADER_DIR) $(CLIBDIR) $(CLIBS)

rev_v2: src/rev_v2.c src/cssarev.c
	$(CC) $(CFLAGS) -o bin/rev_v2 src/rev_v2.c src/cssarev.c -O0 $(CHEADER_DIR) $(CLIBDIR) $(CLIBS)








# STUF THAT NEED TO BE REVISED

all: $(TARGET)

#$(TARGET): src/$(TARGET).c src/ctattack.c
#	$(CC) $(CFLAGS) -o bin/$(TARGET) src/$(TARGET).c src/ctattack.c -O0 $(CHEADER_DIR)

#bin/main: src/$(TARGET).cpp 	# this is not working, how to make bin/main depend on src/$(TARGET)





debug: src/$(TARGET).c src/cssarev.c
	$(CC) $(CFLAGS) -o bin/$(TARGET) $(TARGET).c src/cssarev.c -O0 -g $(CHEADER_DIR)


run: bin/$(TARGET) 
	./bin/$(TARGET) $(ARGS)

clean:
	$(RM) $(TARGET)

