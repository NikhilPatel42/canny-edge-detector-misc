#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main()
{
    // print all files from the current directory and below
    for(auto& p: fs::recursive_directory_iterator(fs::current_path()))
        std::cout << p.path() << '\n';
    // print all files from the current directory
    for(auto& p: fs::directory_iterator(fs::current_path()))
        std::cout << p.path() << '\n';
}
