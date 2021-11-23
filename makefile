all: finalProject.h finalProject.cpp main.cpp
	g++ finalProject.h finalProject.cpp main.cpp -o main
all-GDB: main.cpp
	g++ -g finalProject.h finalProject.cpp main.cpp -o main