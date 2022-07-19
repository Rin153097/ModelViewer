#pragma once

#include <windows.h>
#include <crtdbg.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif

inline LPWSTR hr_trace(HRESULT hr)
{
	LPWSTR msg{ 0 };
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg), 0, NULL);
	return msg;
}

class benchmark
{
	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER startTicks;
	LARGE_INTEGER currentTicks;

public:
	benchmark()
	{
		QueryPerformanceFrequency(&ticksPerSecond);
		QueryPerformanceCounter(&startTicks);
		QueryPerformanceCounter(&currentTicks);
	}
	~benchmark() = default;
	benchmark(const benchmark&) = delete;
	benchmark& operator=(const benchmark&) = delete;
	benchmark(benchmark&&) noexcept = delete;
	benchmark& operator=(benchmark&&) noexcept = delete;

	void begin()
	{
		QueryPerformanceCounter(&startTicks);
	}
	float end()
	{
		QueryPerformanceCounter(&currentTicks);
		return static_cast<float>(currentTicks.QuadPart - startTicks.QuadPart) / static_cast<float>(ticksPerSecond.QuadPart);
	}
};

