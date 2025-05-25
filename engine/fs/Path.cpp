//
// Created by Dquavius Griffin on 5/18/25.
//

#include "Path.h"

#include <__filesystem/filesystem_error.h>

mach::fs::Path::Path() = default;

mach::fs::Path::Path(const std::string& filepath)
{
    this->m_constructedPath = filepath;
}

mach::fs::Path::Path(std::span<const std::string> segments)
{
    for (const auto& segment : segments)
    {
        this->m_constructedPath += mach::fs::Path::separator + segment;
    }
}

void mach::fs::Path::append(const std::string &segment)
{
    this->m_constructedPath += mach::fs::Path::separator + segment;
}

const std::string& mach::fs::Path::toString() const {
    return this->m_constructedPath;
}

std::string mach::fs::Path::working = std::filesystem::current_path().string();

#ifdef PLATFORM_WINDOWS
std::string mach::fs::Path::separator = "\";

#elifdef PLATFORM_APPLE
std::string mach::fs::Path::separator = "/";

#endif