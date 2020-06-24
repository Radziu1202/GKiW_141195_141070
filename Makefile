LIBS=-lGL -lglfw -lGLEW 
HEADERS=stale.h  lodepng.h  shaderprogram.h  OBJ.h klawisze.h  mover.h  material.h
FILES=lodepng.cpp  main.cpp  shaderprogram.cpp  OBJ.cpp  klawisze.cpp  mover.cpp  material.cpp
main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES)  $(LIBS) -I.
