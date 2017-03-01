
# Makefile

include		../Makefile.ini

TARGET		= skeleton

OBJS		= skeleton.o

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(GPINCLUDE)
LDFLAGS		= $(OVERALLLDFLAGS) -L$(GPLIB) -lgp -lm

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<



all: $(TARGET) $(OBJS)

$(TARGET) : $(OBJS) $(GPLIBFILE)
	$(CCC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)



clean:
	rm -f $(OBJS) data.* *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS) $(TARGET)



# File dependencies
skeleton.o: skeleton.cc ../include/gpconfig.h ../include/gp.h



