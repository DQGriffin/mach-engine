//
// Created by Dquavius Griffin on 5/19/25.
//

#ifndef FS_H
#define FS_H
#include "Path.h"
#include <string>
#include <vector>
#include <fstream>

#include "../logging/logging.h"

namespace mach::fs
{
    enum FileStatus
    {
        FILE_DOES_NOT_EXIST = 1,
        NOT_A_DIRECTORY = 2,

    };

    inline bool exists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }

    inline bool exists(const mach::fs::Path& path)
    {
        return std::filesystem::exists(path.toString());
    }

    inline std::vector<unsigned char> readFileAsBytes(const std::string& filepath) {
        // 1. Open the file in binary mode and immediately seek to the end
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);

        if (!file || !file.is_open()) {
            std::cerr << "Failed to open file: " << filepath << std::endl;
            return {};
        }

        // 2. Get the file size and reset the stream pointer to the beginning
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // 3. Allocate a vector large enough to hold all bytes
        std::vector<unsigned char> buffer(size);

        // 4. Read the file data into the vector
        // A reinterpret_cast<char*> is required because ifstream expects char*
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cerr << "Error occurred while reading the file." << std::endl;
            return {};
        }

        return buffer;
    }

}

#endif //FS_H
