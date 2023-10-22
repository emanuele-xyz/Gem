#include <Gem\pch.h>
#include <Gem\Gem.h>
#include <Gem\Utils.h>
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

		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
		spdlog::init_thread_pool(2048, 1);
		auto console_logger{ spdlog::stdout_color_mt<spdlog::async_factory>("console") };
		auto file_logger{ spdlog::basic_logger_mt<spdlog::async_factory>("basic_logger", "log.txt") };
		console_logger->info("Hello!");
		file_logger->info("Hello!");

		auto n_threads{ std::thread::hardware_concurrency() - 2 };
		tf::Executor executor{ n_threads };
		tf::Taskflow taskflow{};

		taskflow.emplace([]() { std::cout << "TaskA\n"; });
		taskflow.emplace([]() { std::cout << "TaskB\n"; });

		executor.run(taskflow).wait();
	}
}
