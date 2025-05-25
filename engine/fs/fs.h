//
// Created by Dquavius Griffin on 5/19/25.
//

#ifndef FS_H
#define FS_H
#include "Path.h"
#include <string>

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
}

#endif //FS_H
