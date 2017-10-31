
# This is where the binaries live
#BINDIR = /users/c22jrb/bin
BINDIR = /usr/local/src/bin

# This is your C compiler
# I strongly recomend gcc for this, I've used 2.7.1 and 2.7.2
CC = gcc
#CC = cc

# These are the flags you need for your C compiler
# -Ae allows the 'long long int' type to be used
# on an HP compiler
#CFLAGS = -Ae
CFLAGS = -Wall

# libraries
LIBS = -lm

# object files
SF2ASCII_OBJECTS = sf2ascii.o
ASCII2SF_OBJECTS = ascii2sf.o strfcns.o

ALL = sf2ascii ascii2sf

all: $(ALL)

sf2ascii: $(SF2ASCII_OBJECTS)
	$(CC) $(CFLAGS) -o sf2ascii $(SF2ASCII_OBJECTS) $(LIBS)

sf2ascii.o: sf2ascii.c
	$(CC) $(CFLAGS) -c sf2ascii.c

ascii2sf: $(ASCII2SF_OBJECTS)
	$(CC) $(CFLAGS) -o ascii2sf $(ASCII2SF_OBJECTS) $(LIBS)

ascii2sf.o: ascii2sf.c strfcns.h
	$(CC) $(CFLAGS) -c ascii2sf.c

strfcns.o: strfcns.c strfcns.h
	$(CC) $(CFLAGS) -c strfcns.c

clean:
	rm *.o
	rm $(ALL)

install:
	cp sf2ascii $(BINDIR)
	cp ascii2sf $(BINDIR)
