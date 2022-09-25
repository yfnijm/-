all: main.cpp partition.cpp partition.hpp
	g++ main.cpp partition.cpp -g -o Lab2 -O3

format:  
	clang-format -style="{IndentWidth: 4,TabWidth: 4}" -i *.cpp *.hpp

clean:
	rm Lab2 
	rm output.txt
