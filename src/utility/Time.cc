#include "Time.h"

#include <iomanip>
#include <iostream>

Time::Time() : _time(Clock::now()) {}

void Time::end() {
    auto dur = std::chrono::duration_cast<Ms>(Clock::now() - _time).count();
    std::cout << std::fixed << std::setprecision(7) << dur / 1000000.0 << "s"
              << std::endl;
}

std::ostream &operator<<(std::ostream &os, Time &time) {
    auto dur =
        std::chrono::duration_cast<Time::Ms>(Time::Clock::now() - time._time)
            .count();
    os << std::setw(8) << std::right << std::fixed << std::setprecision(7);
    os << dur / 1000000.0 << std::endl;
    return os;
}
