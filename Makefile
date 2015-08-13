CC = g++

INSTALLDIR = $(PWD)

CFLAGS = -DCONSOLE -g -O3 -Wall -std=c++11 
CPPFLAGS = -DCONSOLE -g -O3 -Wall -std=c++11 
CPPLIBS = -lpll_optimize -lpll -lm

CPPFILES = $(wildcard **/*.cpp)
CPPFILES = $(shell find jModelTest2/ -type f -name '*.cpp')
OBJFILES = src/main-console.o \
	   src/utils.o \
	   src/model.o \
	   src/msapll.o \
	   src/treepll.o \
	   src/model_optimizer_pll.o \
	   src/model_selection.o \
	   src/modeltest.o 
DEPS = 

all: $(OBJFILES)
	$(CC) $(CFLAGS) -o modeltest $(OBJFILES) $(CPPLIBS)
	@echo $(INSTALLDIR)

jModelTest2/%.o: jModelTest2/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -std=c++11 -c -o $@ $< 

clean:
	rm -rf obj
