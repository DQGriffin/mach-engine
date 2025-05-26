//
// Created by Dquavius Griffin on 5/19/25.
//

#ifndef ARCHIVE_H
#define ARCHIVE_H
#include "../fs/Path.h"
#include "fstream"
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>


namespace mach::archive
{
    struct TocEntry
    {
        std::string name;
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
        uint64_t uncompressedSize;
        bool compressed;
    };

    class Archive
    {
    private:
        std::unordered_map<std::string, TocEntry> toc;
        std::ifstream file;

    public:
        Archive();
        explicit Archive(const mach::fs::Path& path);
        ~Archive();
        bool open();
        void close();
        std::vector<char> get(const std::string& name);
        void unpack(std::string rootDir);
        static void createArchive(const std::string& outputPath, const mach::fs::Path& inputPath, bool recursive);

    private:
        void write(const std::string& name, const std::vector<char>& data);

    };
}




#endif //ARCHIVE_H
