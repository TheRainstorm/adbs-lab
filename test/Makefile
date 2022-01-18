
INCLUDE = -I../src
LIB = -L../src -lBMgr
CXXFLAGS = $(INCLUDE) $(LIB)

test_BMgr.exe: libBMgr.a
	g++ test_BMgr.cpp $(CXXFLAGS) -o $@

libBMgr.a:
	make -C ../src/

clean:
	rm test_BMgr.exe
	make clean -C ../src/