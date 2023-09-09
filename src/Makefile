FLAGS=-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wextra-semi -Wfloat-equal -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo
LIBS=-lglfw -lGLEW -lGL -lSOIL
CORE_FILES=core/*.cpp core/internal/*.cpp
LESSONS_FILES=lessons/*.cpp lessons/helpers/*.cpp

all:
	clang++ -O3 -std=c++20 $(LIBS) $(FLAGS) $(CORE_FILES) $(LESSONS_FILES) main.cpp -o run
