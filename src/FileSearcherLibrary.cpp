#include <iostream>
#include <filesystem>
#include <cstring>
#include "thread.h"
#include "FileSearcherLibrary.h"
namespace fs = std::filesystem;

const int kNUMBER_OF_CORES = 8;
bool FOUND_THE_FILE = false;

std::vector<long long> curr_path_itr(kNUMBER_OF_CORES, 0);
std::vector<long long> end_of_range_for_thread(kNUMBER_OF_CORES, 0);
std::vector<fs::path> subdirectories_pathes;

void FileSearcher::search_for_the_file_in_directory(const char* filename, const char* start_directory){
    fs::path start_path(start_directory);

    // Get the list of subdirectories nearest to 'start_directory'
    get_list_of_subdirectories(start_path, subdirectories_pathes);
    // Divide subdirectories into kNUMBER_OF_CORES groups and set starting points for each group 
    set_starting_points_for_threads(subdirectories_pathes, curr_path_itr);
    // Set end points for every thread (exclusively)
    set_end_points_for_threads(curr_path_itr, end_of_range_for_thread, subdirectories_pathes.size());

    std::vector<std::thread> threads(kNUMBER_OF_CORES);
    // Helps to detect whether the thread finished his cycle
    std::vector<long long> prev_itr_helper(kNUMBER_OF_CORES, -1);
    while (!FOUND_THE_FILE && !checked_all_directories(curr_path_itr, end_of_range_for_thread)) {
        refresh_threads(threads, curr_path_itr, prev_itr_helper, end_of_range_for_thread, subdirectories_pathes, filename, start_directory);
    }

    if (!FOUND_THE_FILE)
        std::cout << "Couldn't find the file!" << std::endl;
}


void FileSearcher::get_list_of_subdirectories(fs::path& path, std::vector<fs::path>& subdirectories) {
    for (const fs::directory_entry& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied))
        subdirectories.push_back(entry.path());
}

void FileSearcher::set_starting_points_for_threads(std::vector<fs::path>& directories, std::vector<long long>& starting_points) {
    if (directories.size() >= kNUMBER_OF_CORES) {
        for (long long i = 1, size_per_thread = directories.size() / kNUMBER_OF_CORES; i < kNUMBER_OF_CORES; i++)
            starting_points[i] = i * size_per_thread;
    } else {
        int i = 0;
        while (i < directories.size()) {
            curr_path_itr[i] = i;
            i++;
        }
        while (i < kNUMBER_OF_CORES) {
            curr_path_itr[i] = directories.size();
            i++;
        }
    }
}

void FileSearcher::set_end_points_for_threads(std::vector<long long>& starting_points, std::vector<long long>& end_points, int number_of_directories) {
    for (int i = 0; i < kNUMBER_OF_CORES - 1; i++)
        end_points[i] = starting_points[i + 1];
    end_points[kNUMBER_OF_CORES - 1] = number_of_directories;
}

bool FileSearcher::checked_all_directories(std::vector<long long>& curr_itr, std::vector<long long> &end_itr) {
    for (int i = 0; i < kNUMBER_OF_CORES; i++)
        if (curr_itr[i] != end_itr[i]) return false;
    return true;
}

void FileSearcher::refresh_threads(std::vector<std::thread>& threads, const std::vector<long long>& curr_itr, std::vector<long long>& prev_itr, const std::vector<long long>& end_itr, const std::vector<fs::path>& pathes, const char* filename, const char* start_dir) {
    for (int i = 0; i < kNUMBER_OF_CORES; i++) {
        // if curr_itr has changed, it means the thread finished his work, so we can use a new thread
        if (curr_itr[i] != prev_itr[i] && curr_itr[i] != end_itr[i]) {
            threads[i] = std::thread(find_file_in_directory, filename, start_dir, pathes[curr_itr[i]], i);
            prev_itr[i] = curr_itr[i];
            threads[i].detach();
        }
    }
}

void FileSearcher::find_file_in_directory(const char* file_to_find, const char* start_dir, const fs::path& curr_directory_path, int subdirectory_index) {
    int NOT_ROOT_NEAREST_SUBFOLDER = -1;
    fs::path curr_path(curr_directory_path);

    // Catch files/folders that are inaccessible for the user
    try {
        auto iterator = fs::directory_iterator(curr_path, fs::directory_options::skip_permission_denied);
    }
    catch (fs::filesystem_error const& ex) {
        if (subdirectory_index != NOT_ROOT_NEAREST_SUBFOLDER) {
            subdirectories_pathes[subdirectory_index].clear();
            curr_path_itr[subdirectory_index]++;
        }
        return;
    }

    // Recursively visit subdirectories or check the files
    for (const fs::directory_entry& entry : fs::directory_iterator(curr_path, fs::directory_options::skip_permission_denied)) {
        if (entry.is_directory())
            find_file_in_directory(file_to_find, start_dir, entry.path(), NOT_ROOT_NEAREST_SUBFOLDER);
        else if (entry.is_regular_file() && strcmp(entry.path().filename().string().c_str(), file_to_find) == 0) {
            std::cout << "Way: " << start_dir << entry.path().relative_path().string() << std::endl;
            FOUND_THE_FILE = true;
        }

        if (FOUND_THE_FILE) break;
    }

    if (subdirectory_index != NOT_ROOT_NEAREST_SUBFOLDER) 
        curr_path_itr[subdirectory_index]++;
    
    if (FOUND_THE_FILE) return;
    return;
}