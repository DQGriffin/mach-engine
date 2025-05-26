//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H
#include "../time/time.h"

namespace mach
{
    class FpsCounter
    {
    private:
        double lastTime = 0.0;
        double fps = 0.0;

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
            fps = 0.9 * fps + 0.1 * (1.0 / delta);
        }

        double getFps() const
        {
            return fps;
        }
    };
}

#endif //FPSCOUNTER_H
