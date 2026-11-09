#include "renderthread.h"

#include <bgfx/bgfx.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

static int const POLL_MILLISECONDS = 10;

static std::thread _Thread;
static bool _Running = false;
static std::uint32_t _Handed = 0;
static std::atomic<bool> _StopRequested = false;
static std::atomic<bool> _Latched = false;
static std::atomic<bool> _ZeroWhenIdle = false;
static std::atomic<std::uint32_t> _Finished = 0;


static void Thread_Body(void)
{
	// Called with no bgfx context, this makes the calling thread the render thread for the next bgfx::init.
	bgfx::renderFrame();
	_Latched.store(true);
	_Latched.notify_one();

	while (!_StopRequested.load()) {
		switch (bgfx::renderFrame(POLL_MILLISECONDS)) {
			case bgfx::RenderFrame::Render:
				_Finished.fetch_add(1);
				break;

			case bgfx::RenderFrame::Timeout:
				if (_ZeroWhenIdle.load()) {
					_Finished.store(0);
					_ZeroWhenIdle.store(false);
					_ZeroWhenIdle.notify_one();
				}
				break;

			case bgfx::RenderFrame::NoContext:
				std::this_thread::sleep_for(std::chrono::milliseconds(POLL_MILLISECONDS));
				break;

			default:
				break;
		}
	}
}


void RenderThread_Start(void)
{
	_StopRequested.store(false);
	_Latched.store(false);
	_Thread = std::thread(Thread_Body);
	_Latched.wait(false);
	_Running = true;
}


// bgfx::init hands over frames of its own, which RenderThread_Note_Frame never counted.
void RenderThread_After_Init(void)
{
	if (!_Running) {
		return;
	}
	_Handed = 0;
	_ZeroWhenIdle.store(true);
	_ZeroWhenIdle.wait(true);
}


void RenderThread_Stop(void)
{
	if (!_Running) {
		return;
	}
	_StopRequested.store(true);
	_Thread.join();
	_Running = false;
}


bool RenderThread_Running(void)
{
	return(_Running);
}


void RenderThread_Note_Frame(void)
{
	_Handed++;
}


bool RenderThread_Busy(void)
{
	return(_Running && _Handed != _Finished.load());
}
