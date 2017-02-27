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

SRC1= persp_cube_image_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ1= persp_cube_image_sdl.o $(GCGDIR)$(GCGLib).o
SRC2= persp_zbuffer.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ2= persp_zbuffer.o $(GCGDIR)$(GCGLib).o

BIN= persp_cube_image_sdl persp_zbuffer

all:$(BIN)

persp_cube_image_sdl:$(OBJ1)
	$(CC) $(OBJ1) $(LDLIB) -o $@

persp_zbuffer:$(OBJ2)
	$(CC) $(OBJ2) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@

persp_cube_image_sdl.o:$(SRC1)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_image_sdl.c

persp_zbuffer.o:$(SRC2)
	$(CC) $(CDEB) $(CFLAGS) persp_zbuffer.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGLib).o $(BIN)
