CXXFLAGS=-std=c++11
all: bin/planificador bin/pcp bin/plp

# Binarios
bin/planificador: src/planificador.o
	$(CXX) -o $@ $^

bin/pcp: src/pcp.o
	$(CXX) -o $@ $^

bin/plp: src/plp.o
	$(CXX) -o $@ $^

# Objetos
src/planificador.o: src/planificador.cpp src/planificador.h src/structs.h bin
src/pcp.o: src/pcp.cpp src/pcp.h bin
src/plp.o: src/plp.cpp src/plp.h	bin

# Utilidades
bin:
	mkdir bin

clean:
	rm -f src/*.o bin/* src/*.*~ src/*~
	rmdir bin

debug: CXXFLAGS += -DDEBUG -g
debug: all