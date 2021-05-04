#pragma once

#include <chrono>
#include <array>
#include <fstream>

template <class Resolution>
class Time {
public:
	using clock_type = std::chrono::high_resolution_clock;
private:
	std::chrono::time_point<clock_type> now;

public:
	Time() {
		set();
	}
	void set() {
		now = clock_type::now();
	}

	unsigned long operator*() {
		auto duration = std::chrono::duration_cast<Resolution>(now.time_since_epoch());
		return duration.count();
	}

	friend long operator-(const Time<Resolution> & t2, const Time<Resolution> & t1) {
		auto duration = std::chrono::duration_cast<Resolution>(t2.now-t1.now);
		return duration.count();
	}
};

using Milliseconds = Time<std::chrono::milliseconds>;
using Microseconds = Time<std::chrono::microseconds>;
using Seconds = Time<std::chrono::seconds>;

enum class TimingLogType {
	StartStop, //Log alternates start and stop times
	AllTimestamps, //Log keeps timestamps, and we want elapsed time between each timestamp
	IndividualTimestamps //Log keeps timestamps, and we just want to print the timestamp values themselves
};

template <class TimeType, unsigned N>
class TimingLog {

private:
	TimingLogType type;
	unsigned position;
	std::array<TimeType,N> log_arr;

public:

	TimingLog (TimingLogType _type) :
		position {0},
		type {_type}
		{}

	void log() {
		if(position == N) return;
		log_arr[position].set();
		++position;
	}

	void reset() {
		position = 0;
	}

	void print(std::ostream & os, bool print_ids = false) {

		if(type == TimingLogType::StartStop) {
			for (int i = 1; i < position; i+=2) {
				if (print_ids) os << i/2 << " ";
				os << (log_arr[i] - log_arr[i-1]) << "\n";
			}
		}

		if(type == TimingLogType::AllTimestamps) {
			for (int i = 1; i < position; ++i) {
				if (print_ids) os << i << " ";
				os << (log_arr[i] - log_arr[i-1]) << "\n";
			}
		}

		if(type == TimingLogType::IndividualTimestamps) {
			for (int i = 0; i < position; ++i) {
				if (print_ids) os << i << " ";
				os << *log_arr[i] << "\n";
			}
		}

	}

	void print(const char * filename, bool print_ids = false) {

		std::ofstream os {filename};
		if(!os) return;
		print(os, print_ids);

	}

};