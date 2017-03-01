
# Makefile for the lawnmower problem

include		../Makefile.ini

TARGET		= lawn

OBJS		= mower.o lawn.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(GPINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(GPLIB) -lgp -lm

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<



all: $(TARGET)

clean:
	rm -f $(OBJS) data.* *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS) $(TARGET)



$(TARGET) : $(OBJS) $(GPLIBFILE)
	$(CCC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)



# File dependencies
lawn.o : lawn.cc ../include/gp.h ../include/gpconfig.h lawn.h 
mower.o : mower.cc ../include/gp.h lawn.h 



