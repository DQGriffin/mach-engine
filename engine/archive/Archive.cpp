//
// Created by Dquavius Griffin on 5/19/25.
//

#include "Archive.h"
#include "../logging/logging.h"
#include "../compression/compress.h"
#include <unordered_map>

mach::archive::Archive::Archive()
{
    LOG_INFO("Archive instantiated");
}

mach::archive::Archive::Archive(const mach::fs::Path &path)
{
    LOG_INFO("Archive instantiated");
    this->file.open(path.toString(), std::ios::binary);
    if (!this->file.is_open()) return;
}

mach::archive::Archive::~Archive()
{
    this->file.close();
    LOG_INFO("closed file");
}

bool mach::archive::Archive::open()
{
    if (!this->file.is_open()) return false;

    // read header
    char magic[4];
    this->file.read(magic, 4);
    if (std::string(magic, 4) != "MPAK")
    {
        LOG_ERROR("archive file is not valid");
        return false;
    }

    uint32_t version;
    this->file.read(reinterpret_cast<char*>(&version), 4);
    if (version != 1)
    {
        LOG_ERROR("archive file version is not supported");
        return false;
    }

    uint64_t toc_offset;
    this->file.read(reinterpret_cast<char*>(&toc_offset), 8);

    uint32_t file_count;
    this->file.read(reinterpret_cast<char*>(&file_count), 4);

    this->file.seekg(toc_offset);
    for (uint32_t i = 0; i < file_count; i++)
    {
        uint16_t name_len;
        this->file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

        std::string name(name_len, '\0');
        this->file.read(name.data(), name_len);

        TocEntry entry;
        entry.name = name;
        this->file.read(reinterpret_cast<char*>(&entry.offset), 8);
        this->file.read(reinterpret_cast<char*>(&entry.uncompressedSize), 8);
        this->file.read(reinterpret_cast<char*>(&entry.compressedSize), 8);
        this->file.read(reinterpret_cast<char*>(&entry.size), 8);
        this->file.read(reinterpret_cast<char*>(&entry.compressed), sizeof(entry.compressed));
        this->toc[name] = entry;

        std::cout << "Entry name: " << entry.name << std::endl;
        std::cout << "Entry offset: " << entry.offset << std::endl;
        std::cout << "Entry uncompressedSize: " << entry.uncompressedSize << std::endl;
        std::cout << "Entry compressedSize: " << entry.compressedSize << std::endl;
        std::cout << "Entry size: " << entry.size << std::endl;
        std::cout << "Entry compressed: " << entry.compressed << std::endl;
    }

    return true;
}

void mach::archive::Archive::close()
{
    this->file.close();
}

bool mach::archive::Archive::contains(const std::string& name) const
{
    return this->toc.contains(name);
}

std::vector<char> mach::archive::Archive::get(const std::string &name)
{
    LOG_INFO(std::format("Looking for '{}' in archive", name));
    if (!this->file.is_open()) throw std::runtime_error("MACH::ARCHIVE::Archive::get::get. File is closed");
    auto it = this->toc.find(name);
    if (it == this->toc.end()) throw std::runtime_error("File now found in archive");

    const TocEntry& entry = it->second;
    this->file.seekg(entry.offset);
    std::vector<char> buffer(entry.size);
    this->file.read(buffer.data(), entry.size);

    if (entry.compressed)
    {
        std::vector<char> output(entry.uncompressedSize);
        mach::compression::decompressLZ4(buffer, output, entry.uncompressedSize);
        return output;
    }

    return buffer;
}

void mach::archive::Archive::unpack(std::string rootDir)
{
    // std::cout << "Unpacking archive" << std::endl;
    for (const auto& pair : this->toc) {
        // std::cout << "Processing " << pair.first << std::endl;
        auto name = pair.first;
        std::string outputPath = rootDir + name;
        std::vector<char> data = this->get(name);
        this->write(outputPath, data);
    }
}

void mach::archive::Archive::createArchive(const std::string& outputPath, const mach::fs::Path& inputPath, bool recursive)
{
    std::ofstream out(outputPath, std::ios::binary);
    if (!out)
    {
        throw std::runtime_error("Failed to create output file");
    }

    // Write placeholder header
    out.write("MPAK", 4);
    uint32_t version = 1;
    out.write(reinterpret_cast<char*>(&version), 4);

    uint64_t toc_offset_placeholder = 0;
    out.write(reinterpret_cast<char*>(&toc_offset_placeholder), 8);

    uint32_t file_count_placeholder = 0;
    out.write(reinterpret_cast<char*>(&file_count_placeholder), 4);

    std::vector<TocEntry> toc;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath.toString()))
    {
        if (!entry.is_regular_file()) continue;

        std::ifstream in(entry.path(), std::ios::binary | std::ios::ate);
        if (!in)
        {
            throw std::runtime_error("Failed to open input file");
        }

        uint64_t uncompressedSize = static_cast<uint64_t>(in.tellg());
        in.seekg(0, std::ios::beg);

        std::vector<uint8_t> inputData(uncompressedSize);
        in.read(reinterpret_cast<char*>(inputData.data()), uncompressedSize);

        std::vector<uint8_t> compressedData;
        bool wasCompressed = mach::compression::compressLZ4(inputData, compressedData);

        TocEntry tocEntry;
        tocEntry.name = std::filesystem::relative(entry.path(), inputPath.toString()).generic_string();
        tocEntry.offset = out.tellp();

        in.seekg(0, std::ios::end);
        tocEntry.uncompressedSize = in.tellg();
        in.seekg(0);

        if (wasCompressed)
        {
            uint64_t compressedSize = compressedData.size();
            tocEntry.compressedSize = compressedSize;
            tocEntry.size = compressedSize;
            tocEntry.compressed = true;
            out.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
        }
        else
        {
            tocEntry.size = uncompressedSize;
            out.write(reinterpret_cast<const char*>(inputData.data()), uncompressedSize);
        }
        toc.push_back(tocEntry);
    }

    // write TOC
    uint64_t toc_offset = out.tellp();
    for (const auto& entry : toc)
    {
        uint16_t name_len = static_cast<uint16_t>(entry.name.size());
        out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        out.write(entry.name.data(), name_len);
        out.write(reinterpret_cast<const char*>(&entry.offset), sizeof(entry.offset));
        out.write(reinterpret_cast<const char*>(&entry.uncompressedSize), sizeof(entry.uncompressedSize));
        out.write(reinterpret_cast<const char*>(&entry.compressedSize), sizeof(entry.compressedSize));
        out.write(reinterpret_cast<const char*>(&entry.size), sizeof(entry.size));
        out.write(reinterpret_cast<const char*>(&entry.compressed), sizeof(entry.compressed));
    }

    // update header
    out.seekp(8);
    out.write(reinterpret_cast<const char*>(&toc_offset), sizeof(toc_offset));

    uint32_t file_count = static_cast<uint32_t>(toc.size());
    out.write(reinterpret_cast<const char*>(&file_count), sizeof(file_count));

    out.close();
}

void mach::archive::Archive::write(const std::string &name, const std::vector<char> &data)
{
    try
    {
        std::filesystem::path filePath = name;
        std::filesystem::create_directories(filePath.parent_path());

        std::ofstream out(filePath.string(), std::ios::binary);
        if (!out)
        {
            throw std::runtime_error("Failed to create output file");
        }
        out.write(reinterpret_cast<const char*>(data.data()), data.size());
        out.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
