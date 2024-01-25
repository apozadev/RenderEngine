#pragma once

#include <chrono>

namespace core
{

class Timer
{
private:
	std::chrono::steady_clock::time_point last;
public:
	Timer();
	long long Mark();
	long long Peek();
};

}
