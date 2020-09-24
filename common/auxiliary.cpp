#include <iostream>
#include "auxiliary.h"

float
randomf()
{
    static __thread std::random_device dev;
    static __thread std::mt19937 rng(dev());
    static __thread std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(rng);
}



