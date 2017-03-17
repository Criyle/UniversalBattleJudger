INCLUDE_PATH = include
CXXFLAGS = -I./include -O2

JUDGER_OBJ = \
	judger/compiler.o \
	judger/manager.o \
	judger/runner.o \
	judger/main.o

JUDGER_HEADERS = \
	judger/common.h \
	judger/env.h \
	judger/okcalls.h\

all: judger

judger: $(JUDGER_OBJ)
	$(CXX) $(CXXFLAGS) $(JUDGER_OBJ) -o judger_bin

$(JUDGER_OBJ) : %.o:%.cpp $(JUDGER_HEADERS) %.h 
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PYONY: clean all
clean:
	rm $(JUDGER_OBJ)
	rm -R ./judger/*~
