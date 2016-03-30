CC = gcc
CFLAGS = -c -Wall
CDEB= -DDEBUG
GCGDIR=../GCGraLib2/
GCGLib=GCGraLib2

# Library linking 
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lm

SRC1= draw_param_curve.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ1= draw_param_curve.o $(GCGDIR)$(GCGLib).o

BIN = draw_param_curve

all:$(BIN)

draw_param_curve:$(OBJ1)
	$(CC) $(OBJ1) $(LDLIB) -o $@

clean:
	rm -f *.o *.*~ *~ $(GCGDIR)*.o $(BIN)