IDIR=lib
CC=gcc
CFLAGS=-g -Wall -lpthread -I$(IDIR)
DFLAGS=-g -Wall -lpthread -I$(IDIR)
FFLAGS=-g -Wall -lpthread -I$(IDIR)
# TFLAGS=-g -Wall -I$(IDIR)

ODIR=obj
SDIR=src

TARGET_SEQ=seq
_DEPS = conditions.h numchecks.h timediff.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = conditions.o numchecks.o timediff.o numcheckseq.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


#data parallelism
TARGET_DATA=data

_DEPS_DATA = conditions.h numchecks.h timediff.h
DEPS_DATA = $(patsubst %,$(IDIR)/%,$(_DEPS_DATA))

_OBJ_DATA = conditions.o numchecks.o timediff.o numcheckdatapar.o
OBJ_DATA = $(patsubst %,$(ODIR)/%,$(_OBJ_DATA))

# #functional parallelism
# TARGET_FUNC=numcheckfuncpar

# _DEPS_FUNC = oven-queue.hpp
# DEPS_FUNC = $(patsubst %,$(IDIR)/%,$(_DEPS_FUNC))

# _OBJ_FUNC = oven-queue.o test.o
# OBJ_FUNC = $(patsubst %,$(ODIR)/%,$(_OBJ_FUNC))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET_SEQ): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

data:
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS_DATA)
	$(CC) -c -o $@ $< $(DFLAGS)
$(TARGET_DATA): $(OBJ_DATA)
	$(CC) -o $@ $^ $(DFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(TARGET)
	rm -f $(TESTER) 2> /dev/null