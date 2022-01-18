
HEADER = BMgr.h defines.h replace_alg.h
SRC = BMgr.cpp replace_LRU.c replace_Clock.c replace_random.c
OBJ = BMgr.o replace_LRU.o replace_Clock.o replace_random.o

libBMgr.a: $(OBJ)
	ar rcs $@ $(OBJ)

clean:
	rm libBMgr.a