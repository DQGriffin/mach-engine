//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef FRAMELIMITER_H
#define FRAMELIMITER_H
#include <thread>

#include "../time/time.h"

namespace mach
{
    class FrameLimiter
    {
    private:
        double lastFrameTime = mach::time::getTime();

    public:
        FrameLimiter() = default;
        ~FrameLimiter() = default;

        void tick()
        {
            double now = mach::time::getTime();
            double delta = now - lastFrameTime;
            double targetFrameTime = 1.0 / 150.0;

            if (delta < targetFrameTime) {
                double sleepTime = targetFrameTime - delta;

                // Convert to microseconds and sleep
                if (sleepTime > 0.002) {
                    std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime - 0.001)); // leave ~1ms for spin
                }

                while (mach::time::getTime() - lastFrameTime < targetFrameTime) {
                    // busy wait
                }

                now = mach::time::getTime(); // remeasure after sleep
                delta = now - lastFrameTime;
            }

            lastFrameTime = now;
        }
    };
}

#endif //FRAMELIMITER_H
