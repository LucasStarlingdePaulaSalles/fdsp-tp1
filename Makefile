IDIR=lib
CC=gcc
CFLAGS=-g -Wall -lpthread -I$(IDIR)
DFLAGS=-g -Wall -lpthread -I$(IDIR)
FFLAGS=-g -Wall -lpthread -I$(IDIR)
# TFLAGS=-g -Wall -I$(IDIR)

ODIR=obj
SDIR=src
BDIR=bin

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

#functional parallelism
TARGET_FUNC=func

_DEPS_FUNC = conditions.h numchecks.h timediff.h
DEPS_FUNC = $(patsubst %,$(IDIR)/%,$(_DEPS_FUNC))

_OBJ_FUNC = conditions.o numchecks.o timediff.o numcheckfuncpar.o
OBJ_FUNC = $(patsubst %,$(ODIR)/%,$(_OBJ_FUNC))

# Test object
TESTER=test

_DEPS_T = 
DEPS_T = $(patsubst %,$(IDIR)/%,$(_DEPS_T))

_OBJ_T = test.o
OBJ_T = $(patsubst %,$(ODIR)/%,$(_OBJ_T))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET_SEQ): $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

data:
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS_DATA)
	$(CC) -c -o $@ $< $(CFLAGS)
$(TARGET_DATA): $(OBJ_DATA)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

func:
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS_FUNC)
	$(CC) -c -o $@ $< $(CFLAGS)
$(TARGET_FUNC): $(OBJ_FUNC)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

test:
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS_T)
	$(CC) -c -o $@ $< $(TFLAGS)
$(TESTER): $(OBJ_T)
	$(CC) -o $(BDIR)/$@ $^ $(TFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(BDIR)/$(TARGET_SEQ)  2> /dev/null
	rm -f $(BDIR)/$(TARGET_DATA) 2> /dev/null
	rm -f $(BDIR)/$(TARGET_FUNC) 2> /dev/null
	rm -f $(BDIR)/$(TESTER) 2> /dev/null