//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H
#include "../time/time.h"
#include <array>
#include <numeric>

namespace mach
{
    class FpsCounter
    {
    private:
        double lastTime = 0.0;
        double fps = 0.0;

        static constexpr int SAMPLE_COUNT = 15;
        std::array<double, SAMPLE_COUNT> fpsSamples{};
        int sampleIndex = 0;
        int samplesFilled = 0;

    public:
        void tick()
        {
            double now = mach::time::getTime();
            if (lastTime == 0.0) {
                lastTime = now;
                return;
            }

            double delta = now - lastTime;
            lastTime = now;
            double currentFps = 1.0 / delta;
            fps = 0.9 * fps + 0.1 * currentFps;

            fpsSamples[sampleIndex] = currentFps;
            sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT;
            if (samplesFilled < SAMPLE_COUNT) {
                samplesFilled++;
            }
        }

        double getFps() const
        {
            return fps;
        }

        double average() const
        {
            if (samplesFilled == 0) return 0.0;
            double sum = std::accumulate(fpsSamples.begin(), fpsSamples.begin() + samplesFilled, 0.0);
            return sum / samplesFilled;
        }
    };
}

#endif //FPSCOUNTER_H
