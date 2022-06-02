#pragma once
#include <vector>
#include <filesystem>
#include "thread.h"
namespace fs = std::filesystem;

namespace FileSearcher {
    void search_for_the_file_in_directory(const char* filename, const char* start_directory);
    void get_list_of_subdirectories(fs::path& path, std::vector<fs::path>& subdirectories);
    void set_starting_points_for_threads(std::vector<fs::path>& directories, std::vector<long long>& starting_points);
    void set_end_points_for_threads(std::vector<long long>& starting_points, std::vector<long long>& end_points, int number_of_directories);
    bool checked_all_directories(std::vector<long long>& curr_itr, std::vector<long long>& end_itr);
    void refresh_threads(std::vector<std::thread>& threads, const std::vector<long long>& curr_itr, std::vector<long long>& prev_itr, const std::vector<long long>& end_itr, const std::vector<fs::path>& pathes, const char* filename, const char* start_dir);
    void find_file_in_directory(const char* file_to_find, const char* start_dir, const fs::path& curr_directory_path, int subdirectory_index);
}