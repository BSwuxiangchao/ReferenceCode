#pragma once

#include <thread>
#include <chrono>

class Timer 
{
public:
	template<typename T>
	void SetTimeout(T function, int delay);

	template<typename T>
	void SetInterval(T function, int interval);

	void Stop();
private:
	bool clear = false;
};

template<typename T>
void Timer::SetTimeout(T function, int delay) 
{
	this->clear = false;
	std::thread t([=]() {
		if (this->clear) return;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		if (this->clear) return;

		function();
	});

	t.detach();
}


template<typename T>
void Timer::SetInterval(T function, int interval) 
{
	this->clear = false;
	std::thread t([=]() {
		while (true) {
			if (this->clear) return;
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			if (this->clear) return;
			function();
		}
	});

	t.detach();
}

void Timer::Stop() 
{
	this->clear = true;
}


template<typename callable, class... arguments>
void InvokeSync(int delayMilliseconds, callable&& f, arguments&&... args)
{
	std::function<typename std::result_of<callable(arguments...)>::type()> task
	(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

	std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));

	task();
}


template<typename callable, class... arguments>
void InvokeAsync(int delayMilliseconds, callable&& f, arguments&&... args)
{
	std::function<typename std::result_of<callable(arguments...)>::type()> task
	(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

	std::thread([delayMilliseconds, task]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
		task();
	}).detach();
}
