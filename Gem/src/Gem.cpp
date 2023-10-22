#include <Gem\pch.h>
#include <Gem\Gem.h>
#include <taskflow\taskflow.hpp>
#include <spdlog\spdlog.h>
#include <spdlog\async.h>
#include <spdlog\sinks\basic_file_sink.h>
#include <spdlog\sinks\stdout_color_sinks.h>

namespace Gem
{
	void Hello()
	{
		std::cout << "Hello from Gem!" << std::endl;

		spdlog::init_thread_pool(2048, 1);
		auto console_logger{ spdlog::stdout_color_mt<spdlog::async_factory>("console") };
		console_logger->info("Henlo!");

		auto n_threads{ std::thread::hardware_concurrency() - 2 };
		tf::Executor executor{ n_threads };
		tf::Taskflow taskflow{};

		taskflow.emplace([]() { std::cout << "TaskA\n"; });
		taskflow.emplace([]() { std::cout << "TaskB\n"; });

		executor.run(taskflow).wait();
	}
}
