#ifndef TIMER
#define TIMER
#include <chrono>
class Timer {
    std::chrono::high_resolution_clock::time_point startT;
    std::chrono::high_resolution_clock::time_point endT;
    double dt;

  public:
    void start() {
        startT = std::chrono::high_resolution_clock::now();
    }
    double getDiff() {
        endT = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration<double, std::milli>(endT - startT).count();
        return dt;
    }
};
#endif
