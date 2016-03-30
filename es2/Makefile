# -- makefile
#    if you  encounter the message: "Missing separator..."
#    make sure that all indented lines are indented by using a TAB
#    rather than blanks

CC = gcc
CFLAGS = -c -Wall
CDEB= -DDEBUG
GCGDIR=../GCGraLib2/
GCGLib=GCGraLib2

# Library linking 
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lm

# ---- Files --- 

SRC4= draw_bezier_curve.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ4= draw_bezier_curve.o $(GCGDIR)$(GCGLib).o

BIN= draw_bezier_curve

all:$(BIN) 

draw_bezier_curve:$(OBJ4)
	$(CC) $(OBJ4) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o $@

draw_bezier_curve.o:$(SRC4)
	$(CC) $(CDEB) $(CFLAGS) draw_bezier_curve.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGDIR)*.o $(BIN)
