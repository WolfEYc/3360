all: compile run

compile:
	g++ main.cpp -lpthread -o main

run:
	./main
