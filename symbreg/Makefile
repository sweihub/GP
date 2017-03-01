
# Makefile for the symbolic regression problem

include		../Makefile.ini

TARGET		= sym

OBJS		= symbreg.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(GPINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(GPLIB) -lgp -lm 

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<



all: $(TARGET)

clean:
	rm -f $(OBJS) result.* *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS) $(TARGET)



$(TARGET) : $(OBJS) $(GPLIBFILE)
	$(CCC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)



# File dependencies
symbreg.o: symbreg.cc ../include/gpconfig.h ../include/gp.h



