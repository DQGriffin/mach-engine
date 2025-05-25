//
// Created by Dquavius Griffin on 5/18/25.
//

#ifndef PATH_H
#define PATH_H

#include <iostream>
#include <string>
#include <span>
#include <filesystem>

namespace mach::fs
{
    class Path
    {
    public:
        static std::string documents;
        static std::string working;
        static std::string home;
        static std::string user;
        static std::string downloads;
        static std::string temp;
        static std::string root;
        static std::string separator;
    public:
        Path();
        explicit Path(const std::string& filepath);
        explicit Path(std::span<const std::string> segments);
        void append(const std::string& segment);
        [[nodiscard]] const std::string& toString() const;
    private:
        std::string m_constructedPath;
    };
}



#endif //PATH_H
