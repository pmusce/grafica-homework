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
SRC0= persp_mesh.c viewtransf.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ0= persp_mesh.o viewtransf.o $(GCGDIR)$(GCGLib).o
TESTSRC= viewup_test.c viewtransf.c
TESTOBJ= viewup_test.o viewtransf.o

BIN= persp_mesh

all:$(BIN)

test:$(TESTOBJ)
	$(CC) $(TESTOBJ) $(LDLIB) -o $@

test.o:$(TESTSRC)
	$(CC) $(CDEB) $(CFLAGS) viewup_test.c

persp_mesh:$(OBJ0)
	$(CC) $(OBJ0) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@

persp_mesh.o:$(SRC0)
	$(CC) $(CDEB) $(CFLAGS) persp_mesh.c

viewtransf.o:$(SRC0)
	$(CC) $(CDEB) $(CFLAGS) viewtransf.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGLib).o $(BIN)
