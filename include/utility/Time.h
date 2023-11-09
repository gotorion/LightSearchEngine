#ifndef __TIME_H__
#define __TIME_H__

#include <chrono>
#include <ostream>

class Time {
    friend std::ostream &operator<<(std::ostream &os, Time &time);

   private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Ms = std::chrono::microseconds;

   public:
    Time();
    void end();

   private:
    TimePoint _time;
};

#endif
