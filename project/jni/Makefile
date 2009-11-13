#
# The generateData prog generate makefile:
#

CC	= g++
CFLAGS	= -Wall -W -O2 -g

PROGS	= generateData
OBJS	= GenerateData.o  Global.o

all: depend $(PROGS)

.PHONY : depend
depend: $(patsubst %.o,%.cpp,$(filter %.o,$(OBJS)))
	@$(CC) -MM $(CFLAGS) -I.. $^ 1> .depend


clean: 
	-rm -f *.o $(PROGS) .depend

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $*.o $<

$(PROGS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^) 

ifneq ($(wildcard .depend),)
include .depend
endif
