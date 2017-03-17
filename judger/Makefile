CXX = g++-5
CXXFLAGS = -std=c++14 -g -Wall -MMD
OBJECTS = compiler.o main.o manager.o runner.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = judger

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY : clean
clean:
	rm ${DEPENDS} ${OBJECTS} ${EXEC}
