file_searcher: main.o FileSearcherLibrary.o
	g++ main.o FileSearcherLibrary.o -o file_searcher -I include

main.o: src/main.cpp
	g++ --std=c++17 -c src/main.cpp -I include

FileSearcherLibrary.o: src/FileSearcherLibrary.cpp
	g++ --std=c++17 -c src/FileSearcherLibrary.cpp -I include

clean:
	rm *.o file_searcher