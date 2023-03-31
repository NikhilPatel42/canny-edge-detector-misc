#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
 
namespace fs = std::filesystem;
 
int main()
{
    // Path to the directory
    std::string path
        = "/home/school/canny-edge-detector-misc/lizard/";
 
    // This structure would distinguish a file from a
    // directory
    struct stat sb;
 
    // Looping until all the items of the directory are
    // exhausted
    for (const auto& entry : fs::directory_iterator(path)) {
 
        // Converting the path to const char * in the
        // subsequent lines
        fs::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();
 
        // Testing whether the path points to a
        // non-directory or not If it does, displays path
        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
            std ::cout << path << std::endl;
    }
}
