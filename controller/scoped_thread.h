#pragma once

#include <thread>

/*
	Code inspired by Williams Listing 2.6:
	scoped_thread is a class that holds a thread.
	It checks if the thread is joinable and joins it in the destructor.
	This RAII technique guarantees threads are joined when they leave scope.
*/

class scoped_thread {
private:
	std::thread t;

public:
	explicit scoped_thread(std::thread t_) : t(std::move(t_)) {}
	~scoped_thread() {
		if (t.joinable()) {
			t.join();
		}
	}
	scoped_thread(scoped_thread const &) = delete;
	scoped_thread & operator=(scoped_thread const &) = delete;

	scoped_thread(scoped_thread && other) noexcept : t(std::move(other.t)) {}
	scoped_thread & operator=(scoped_thread && other) noexcept {
		t = std::move(other.t);
		return *this;
	}

};