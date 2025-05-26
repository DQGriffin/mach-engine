//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef TIME_H
#define TIME_H

#include <chrono>

namespace mach::time
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    inline double getTime() {
        using namespace std::chrono;
        static auto start = high_resolution_clock::now();
        auto now = high_resolution_clock::now();
        duration<double> elapsed = now - start;
        return elapsed.count();
    }
}

#endif //TIME_H
