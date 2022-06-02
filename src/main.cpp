#include "FileSearcherLibrary.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3){
        std::cout << "Invalid usage: ./file_searcher FILE_TO_SEARCH.EXTENSION ROOT_DIRECTORY" << std::endl;
        std::cout << "Example: ./file_searcher thread.h D:\\" << std::endl;
        return 0;
    }
    FileSearcher::search_for_the_file_in_directory(argv[1], argv[2]);
    return 0;
}