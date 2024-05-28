SDL=`sdl2-config --cflags --libs`

SOURCES=src/main.cpp\
src/figure.cpp\
src/box.cpp\
src/quat.cpp\
src/scene.cpp\
src/sphere.cpp\
src/tetrahedron.cpp\
src/vector.cpp

all:
	g++ -o program $(SOURCES) $(SDL) -fopenmp

run: all
	./program