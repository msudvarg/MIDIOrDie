#pragma once

#include <chrono>

template <class Resolution>
class Time {
public:
	using clock_type = std::chrono::high_resolution_clock;
private:
	std::chrono::time_point<clock_type> now;

public:
	Time() {
		now = clock_type::now();
	}

	friend long operator-(const Time<Resolution> & t2, const Time<Resolution> & t1) {
		auto duration = std::chrono::duration_cast<Resolution>(t2.now-t1.now);
		return duration.count();
	}
};

using Milliseconds = Time<std::chrono::milliseconds>;
using Microseconds = Time<std::chrono::microseconds>;
using Seconds = Time<std::chrono::seconds>;
