#include <Gem\pch.h>
#include <Gem\Gem.h>
#include <Gem\Utils.h>
#include <taskflow\taskflow.hpp>
#include <spdlog\spdlog.h>
#include <spdlog\async.h>
#include <spdlog\sinks\basic_file_sink.h>
#include <spdlog\sinks\stdout_color_sinks.h>

#include <directxtk\SimpleMath.h>
using Vector3 = DirectX::SimpleMath::Vector3;

namespace Gem::SpdLog
{
	class Handle
	{
	private:
		constexpr static int s_queue_size{ 2048 };
		constexpr static int s_worker_threads{ 1 };
	public:
		static int GetQueueSize() { return s_queue_size; }
		static int GetWorkerThreads() { return s_worker_threads; }
	public:
		Handle(bool log_to_console, bool log_to_file, const std::string& log_file = "log.txt");
		~Handle();
		Handle(const Handle&) = delete;
		Handle(Handle&&) = delete;
		Handle& operator=(const Handle&) = delete;
		Handle& operator=(Handle&&) = delete;
	};

	static Handle* s_handle{ nullptr };

	Handle::Handle(bool log_to_console, bool log_to_file, const std::string& log_file)
	{
		GemAssert(!s_handle);
		s_handle = this;

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
	Handle::~Handle()
	{
		s_handle = nullptr;

		spdlog::shutdown(); // NOTE: necessary for spdlog not to crash on program exit
	}
}

namespace Gem::Taskflow
{
	class Handle
	{
	public:
		Handle(size_t n_threads);
		~Handle();
		Handle(const Handle&) = delete;
		Handle(Handle&&) = delete;
		Handle& operator=(const Handle&) = delete;
		Handle& operator=(Handle&&) = delete;
	public:
		tf::Executor executor;
	};

	static Handle* s_handle{};

	Handle* GetHandle() { GemAssert(s_handle); return s_handle; }

	Handle::Handle(size_t n_threads)
		: executor{ n_threads }
	{
		GemAssert(!s_handle);
		s_handle = this;
	}
	Handle::~Handle()
	{
		s_handle = nullptr;
	}
}

namespace Gem
{
	// TODO: Events
	// Events are generated by the game simulation and are sent to views

	struct GameEvent
	{

	};
}

namespace Gem
{
	// TODO: Commands
	// Commands are generated by views and are sent to the game simulation

	struct ViewCommand
	{

	};
}

namespace Gem
{
	struct GameObject
	{
		Vector3 position;
	};

	class GameSimulation
	{
	public:
		GameSimulation();
		~GameSimulation() = default;
		GameSimulation(const GameSimulation&) = delete;
		GameSimulation(GameSimulation&&) = delete;
		GameSimulation& operator=(const GameSimulation&) = delete;
		GameSimulation& operator=(GameSimulation&&) = delete;
	private:
		GameObject object;
		// TODO: game state
		// TODO: command interpreter
		// TODO: process manager
		// TODO: events
	};

	GameSimulation::GameSimulation()
	{
		object.position = Vector3::Zero;
	}
}

namespace Gem
{
	class GameView
	{
	public:
		virtual ~GameView() {}
	public:
		virtual std::span<const ViewCommand> GetViewCommands() = 0;
		virtual void ProcessGameEvents(std::span<const GameEvent> events) = 0;
	};

	class PlayerView : public GameView
	{
	public:
		PlayerView();
		~PlayerView() = default;
		PlayerView(const PlayerView&) = delete;
		PlayerView(PlayerView&&) = delete;
		PlayerView& operator=(const PlayerView&) = delete;
		PlayerView& operator=(PlayerView&&) = delete;
	public:
		std::span<const ViewCommand> GetViewCommands() override;
		void ProcessGameEvents(std::span<const GameEvent> events) override;
	private:
		// TODO: options
		// TODO: graphics
		// TODO: audio
		// TODO: input interpreter (translates inputs into commands)
		// TODO: commands
		// TODO: process manager
	};

	PlayerView::PlayerView()
	{
	}
	std::span<const ViewCommand> PlayerView::GetViewCommands()
	{
		// TODO: to be implemented
		return {};
	}
	void PlayerView::ProcessGameEvents(std::span<const GameEvent> events)
	{
	}
}

namespace Gem
{
	class Game
	{
	public:
		Game();
		~Game() = default;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;
	private:
		std::unique_ptr<SpdLog::Handle> m_spdlog_handle;
		std::unique_ptr<Taskflow::Handle> m_taskflow_handle;
		std::unique_ptr<GameSimulation> m_game_simulation;
		std::unique_ptr<GameView> m_game_views;
		GameView* m_player_view;
	};

	Game::Game()
	{
		m_spdlog_handle = std::make_unique<SpdLog::Handle>(true, false); // TODO: get parameters from somewhere
		m_taskflow_handle = std::make_unique<Taskflow::Handle>(std::thread::hardware_concurrency() - SpdLog::Handle::GetWorkerThreads() - 1);
	}
}

namespace Gem
{
	void Hello()
	{
		Game game{};

		spdlog::info("Gem Game started!");

		tf::Taskflow taskflow{};
		taskflow.emplace([]() { spdlog::info("TaskA"); });
		taskflow.emplace([]() { spdlog::info("TaskB"); });

		Taskflow::GetHandle()->executor.run(taskflow).wait();
	}
}
