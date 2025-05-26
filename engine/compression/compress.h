//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef COMPRESS_H
#define COMPRESS_H

#include "lz4.h"
#include <vector>

namespace mach::compression
{
    inline bool compressLZ4(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
        int maxCompressedSize = LZ4_compressBound(input.size());
        output.resize(maxCompressedSize);

        int compressedSize = LZ4_compress_default(
            reinterpret_cast<const char*>(input.data()),
            reinterpret_cast<char*>(output.data()),
            static_cast<int>(input.size()),
            maxCompressedSize
        );

        if (compressedSize <= 0) return false;

        float ratio = 100.0f * compressedSize / input.size();
        LOG_DEBUG(std::format("Compression ratio: {}% of original size", ratio));
        output.resize(compressedSize); // shrink to actual
        return true;
    }

    inline bool decompressLZ4(const std::vector<char>& input, std::vector<char>& output, int originalSize) {
        output.resize(originalSize);

        int decompressedSize = LZ4_decompress_safe(
            input.data(),
            output.data(),
            static_cast<int>(input.size()),
            originalSize
        );

        return decompressedSize == originalSize;
    }
}

#endif //COMPRESS_H
