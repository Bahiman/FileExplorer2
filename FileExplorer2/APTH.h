#pragma once
#include <cstdint>
#include <thread>

class APTH
{
	static uint32_t get_recommended_thread_count()
	{
		const uint32_t thread_count = std::thread::hardware_concurrency();

		if (thread_count == 1)
			return 1;
		if (thread_count <= 8)
			return static_cast<uint32_t>(thread_count / 1.5);
		return thread_count / 2;
	}

	APTH() = delete;
public:
	const static inline uint32_t rec_t_count = get_recommended_thread_count();
};
