#pragma once

class Timer
{
public:
	Timer()
	{
		reset();
	}

	int64_t query()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_ts).count();
	}

	void print_init_message(std::string_view build_string)
	{
		FB2K_console_formatter() << fmt::format("{}: initialised in {} ms", build_string, query());
	}

	void print_profiler_message(std::string_view name)
	{
		FB2K_console_formatter() << fmt::format("Profiler ({}): {} ms", name, query());
	}

	void reset()
	{
		m_ts = std::chrono::steady_clock::now();
	}

private:
	std::chrono::steady_clock::time_point m_ts;
};
