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
	class SPDLogHandle
	{
	public:
		constexpr static int s_queue_size{ 2048 };
		constexpr static int s_worker_threads{ 1 };
	public:
		SPDLogHandle(bool log_to_console, bool log_to_file, const std::string& log_file = "log.txt");
	};

	SPDLogHandle::SPDLogHandle(bool log_to_console, bool log_to_file, const std::string& log_file)
	{
		spdlog::init_thread_pool(s_queue_size, s_worker_threads);

		std::vector<spdlog::sink_ptr> sinks{};

		if (log_to_console)
		{
			auto sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
			sinks.push_back(sink);
		}
		if (log_to_file)
		{
			auto sink{ std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true) };
			sinks.push_back(sink);
		}

		auto logger{ std::make_shared<spdlog::async_logger>("logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block) };
		spdlog::set_default_logger(logger);

		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
	}

	class TaskflowHandle
	{
	public:
		TaskflowHandle(size_t n_threads);
	public:
		tf::Executor executor;
	};

	TaskflowHandle::TaskflowHandle(size_t n_threads)
		: executor{ n_threads }
	{
	}
}

namespace Gem
{
	static std::unique_ptr<SPDLogHandle> s_spdlog_handle{};
	static std::unique_ptr<TaskflowHandle> s_taskflow_handle{};

	void Hello()
	{
		s_spdlog_handle = std::make_unique<SPDLogHandle>(true, false);
		spdlog::info("Gem initialization started");

		s_taskflow_handle = std::make_unique<TaskflowHandle>(std::thread::hardware_concurrency() - s_spdlog_handle->s_worker_threads - 1);
		spdlog::info("Taskflow initialized");

		tf::Taskflow taskflow{};
		taskflow.emplace([]() { /*spdlog::info("TaskA");*/ });
		taskflow.emplace([]() { /*spdlog::info("TaskB");*/ });

		s_taskflow_handle->executor.run(taskflow).wait();
	}
}
