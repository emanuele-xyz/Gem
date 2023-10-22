#include <Gem\pch.h>
#include <Gem\Gem.h>
#include <taskflow\taskflow.hpp>

namespace Gem
{
	void Hello()
	{
		std::cout << "Hello from Gem!" << std::endl;

		auto n_threads{ std::thread::hardware_concurrency() - 1 };
		tf::Executor executor{ n_threads };
		tf::Taskflow taskflow{};

		auto A{ taskflow.emplace([]() { std::cout << "TaskA" << std::endl; }) };
		auto B{ taskflow.emplace([]() { std::cout << "TaskB" << std::endl; }) };

		executor.run(taskflow).wait();
	}
}
