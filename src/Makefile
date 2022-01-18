
HEADER = BMgr.h DSMgr.h defines.h replace_alg.h
SRC = BMgr.cpp DSMgr.cpp replace_LRU.c replace_Clock.c replace_random.c
OBJ = BMgr.o DSMgr.o replace_LRU.o replace_Clock.o replace_random.o

libBMgr.a: $(OBJ)
	ar rcs $@ $(OBJ)

clean:
	rm libBMgr.a