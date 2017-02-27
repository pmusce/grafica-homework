# -- makefile
#    if you  encounter the message: "Missing separator..."
#    make sure that all indented lines are indented by using a TAB
#    rather than blanks

CC = gcc
CFLAGS = -c -Wall
CDEB= #-DDEBUG
GCGDIR=../GCGraLib2/
GCGLib=GCGraLib2

# Library linking
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lm
LDLIBGL = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lGL -lGLU -lm

# ---- Files ---

SRC3= tri_rast.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h triangle_rast.h triangle_rast.c
OBJ3= tri_rast.o $(GCGDIR)$(GCGLib).o triangle_rast.o

BIN= tri_rast

all:$(BIN)

tri_rast:$(OBJ3)
	$(CC) $(OBJ3) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@

tri_rast.o:$(SRC3)
	$(CC) $(CDEB) $(CFLAGS) tri_rast.c

triangle_rast.o: triangle_rast.h triangle_rast.c
	$(CC) $(CFLAGS) triangle_rast.c -o$@

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGLib).o $(BIN)
