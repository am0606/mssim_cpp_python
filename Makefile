EXECUTABLE = mssim
CXX = g++
PYPATH = /home/amink/anaconda3/include/python3.7m

SOURCES =  \
 main.cpp \
 mssim.cpp

CXFLAGS = -O3 
LDFLAGS = -O3
DEFINES = 

OBJECTS=$(SOURCES:.cpp=.o)

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(DEFINES) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(DEFINES) $(CXFLAGS) -c $< -o $@

pymodule:
	swig -python -l mssim.i
	g++ -g -fPIC -c mssim.cpp mssim_wrap.c -I$(PYPATH)
	g++ -g -shared mssim.o mssim_wrap.o -o _mssim.so -lstdc++ -lpython3.7m

clean:
	rm -rf *.o
	if [ -f _mssim.so ]; then rm _mssim.so; fi
	if [ -f mssim_wrap.c ];then rm mssim_wrap.c; fi
	if [ -f mssim.py ];then rm mssim.py; fi
	if [ -f mssim ];then rm mssim; fi
