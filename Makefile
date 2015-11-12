CC = g++

INSTALLDIR = $(PWD)

CFLAGS = -DCONSOLE -g -O3 -Wall -std=c++11 
CPPFLAGS = -DCONSOLE -g -O3 -Wall -std=c++11 
CPPLIBS = -lpll_optimize -lpll -lm

CPPFILES = $(wildcard **/*.cpp)
CPPFILES = $(shell find jModelTest2/ -type f -name '*.cpp')
OBJFILES = src/modeltest.o \
	   src/utils.o \
	   src/model.o \
	   src/msapll.o \
	   src/partition.o \
	   src/partitioning_scheme.o \
		 src/testui.o \
	   src/treepll.o \
	   src/model_optimizer_pll.o \
	   src/model_selection.o \
	   src/main.o
     # src/main_console.o
			
DEPS = 

all: $(OBJFILES)
	$(CC) $(CFLAGS) -o modeltest $(OBJFILES) $(CPPLIBS)
	@echo $(INSTALLDIR)

src/%.o: src/%.cpp $(DEPS)
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -rf src/*.o
