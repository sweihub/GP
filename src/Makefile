
# Makefile for the gpc++ kernel

include		../Makefile.ini



TARGET		= $(GPLIBFILE)

GPKERNEL= 	compare.o \
		config.o \
		contain.o \
		create.o \
		cross.o \
		eval.o \
		gene.o \
		generate.o \
		gprand.o \
		gpv.o \
		gp.o \
		loadsave.o \
		misc.o \
		mutate.o \
		node.o \
		pop.o \
		select.o 

OBJS		= $(GPKERNEL) 

CPPFLAGS	= $(OVERALLCPPFLAGS) -I$(GPINCLUDE) 

# Implicit rule how to compile
.cc.o:
	$(CCC) $(CPPFLAGS) -c $<



# Create the GP library. Use either ld (for dynamic libraries) or ar
# and ranlib.
$(TARGET) : $(OBJS) 
	$(AR) -r $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
#	$(LD) -r -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) *~

superclean: clean
	rm -f $(TARGET)

strip:
	strip $(OBJS)
	make



# File dependencies
compare.o: compare.cc ../include/gp.h
config.o: config.cc ../include/gpconfig.h
contain.o: contain.cc ../include/gp.h
create.o: create.cc ../include/gp.h
cross.o: cross.cc ../include/gp.h
eval.o: eval.cc ../include/gp.h
gene.o: gene.cc ../include/gp.h
generate.o: generate.cc ../include/gp.h
gp.o: gp.cc ../include/gp.h
gprand.o: gprand.cc ../include/gp.h
gpv.o: gpv.cc ../include/gp.h
loadsave.o: loadsave.cc ../include/gp.h
misc.o: misc.cc ../include/gp.h
mutate.o: mutate.cc ../include/gp.h
node.o: node.cc ../include/gp.h
pop.o: pop.cc ../include/gp.h
select.o: select.cc ../include/gp.h
