#pragma once

// measure gpu time
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>
// memory usage
#include <windows.h>
#include <psapi.h> 
#include <functional>

using MemoryUsageFunction = std::function<size_t()>;

class Benchmark
{
	inline static double lastTime = glfwGetTime();
	inline static int frameCount = 0;
	inline static int fps = 0;

	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<double, std::milli> frameTime;

	GLuint64 gpuTimeElapsed = 0;

public:

	 GLuint query;

	void InitTimerQuery() {
		glGenQueries(1, &query);
	}
	void BeginQuery() {
		// measures the execution time of commands on the GPU
		glBeginQuery(GL_TIME_ELAPSED, query);
	}
	void MeasureGPUTime() {
		glEndQuery(GL_TIME_ELAPSED);

		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &gpuTimeElapsed);
		std::cout << "[GPU Rendering Time: " << (gpuTimeElapsed / 1e6) << " ms]" << std::endl;
	}

	void StartMeasureFrameTime()
	{
		// measures the execution time of the entire frame on the CPU
		start = std::chrono::high_resolution_clock::now();
	}

	void EndMeasureFrameTime() {
		end = std::chrono::high_resolution_clock::now();
		frameTime = end - start;

		std::cout << "[Frame rendering time: " << frameTime.count() << " ms]" << std::endl;
	}

	static void CalculateFPS() {
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - lastTime >= 1.0) { // per second
			fps = frameCount;	
			frameCount = 0;
			lastTime = currentTime;
		}
		std::cout << "[FPS: " << fps << "]" << std::endl;
	}


	static size_t GetMemoryUsage_Windows() {
		PROCESS_MEMORY_COUNTERS memCounter;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter))) {
			return memCounter.WorkingSetSize / (1024 * 1024); // MB
		}
		return 0;
	}

	static void PrintCPUUsage() {
		unsigned int cores = GetCPUCoreCount();
		std::cout << "[Number of available cores CPU: " << cores << "]" << std::endl;
	}

	static void PrintMemoryUsage(MemoryUsageFunction getMemoryUsage)
	{
		std::cout << "[Memory usage: " << getMemoryUsage() << " MB]" << std::endl;
	}

	static unsigned int GetCPUCoreCount() {
		return std::thread::hardware_concurrency();
	}


	void Update() {
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - lastTime >= 1.0) {
			fps = frameCount;
			frameCount = 0;
			lastTime = currentTime;
		}

		size_t memoryUsage = GetMemoryUsage_Windows();

		std::cout << std::fixed << std::setprecision(2);
		std::cout << "[FPS: " << fps
			<< "] [Frame Time: " << frameTime << " ms] "
			<< "[GPU Time: " << (gpuTimeElapsed / 1e6) << " ms] "
			<< "[Memory: " << memoryUsage << " MB] "
			<< "[CPU Cores: " << GetCPUCoreCount() << "]"
			<< std::endl;
	}
};

