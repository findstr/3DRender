#include <iostream>
#include "auxiliary.h"

float
randomf()
{
    static thread_local std::random_device dev;
    static thread_local std::mt19937 rng(dev());
    static thread_local std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(rng);
}



