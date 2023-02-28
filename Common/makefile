#!smake 

#-----------------------------------------
#Basic Stuff -----------------------------
CC          = g++
cc          = gcc

#-----------------------------------------
#Archatecture ----------------------------
ARCH_OPT  = 
ARCH_DIR  = LINUX

#-----------------------------------------
#Misc Flags ------------------------------
MISC_FLAGS =

#-----------------------------------------
#Optimization ----------------------------
OPT   = -O3

#-----------------------------------------
# Debug mode -----------------------------
#DEBUG   = -s
#DEBUG   = -g3
#DEBUG   = -g
#DEBUG   = -g -DDEBUG -DRWBOUNDS_CHECK -DRWDEBUG=1
#DEBUG = -g

LIB_MOD = 

#-----------------------------------------
#-----------------------------------------

TARGETS = libcommon.a

OBJECTS = algebra.o errors.o load_node.o load_neutral.o timing.o


#-----------------------------------------

default: $(TARGETS)


clean: 
	/bin/rm -r *.o libcommon.a

#-----------------------------------------
#-----------------------------------------

libcommon.a: $(OBJECTS)
	ar cr libcommon.a $(OBJECTS)
	ranlib libcommon.a

#-----------------------------------------

.C.o: 
	$(CC) $(CCOPTS) -c $< 

.cpp.o: 
	$(CC) $(CCOPTS) -c $< 

.c.o: 
	$(cc) $(CCOPTS) -c $< 

.o: $(OBJECTS)
	$(CC) $(LDOPTS) $(OBJS) $(OBJECTS) $< $(LIBS) -o $@

.C: $(OBJECTS)
	$(CC) $(LDOPTS) $(OBJS) $(OBJECTS) $< $(LIBS) -o $@

#-----------------------------------------
#-----------------------------------------





