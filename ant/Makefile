
# Makefile for the artificial ant problem

include		../Makefile.ini

TARGET		= ant

OBJS		= ant.o trail.o

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
ant.o: ant.cc ../include/gp.h ../include/gpconfig.h ant.h
trail.o: trail.cc ../include/gp.h ant.h


