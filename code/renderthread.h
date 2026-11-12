#pragma once

// The thread that owns the graphics device while [Video] RenderThread is on; docs/RENDER_THREAD.md describes it.
// Every function is called from the main thread.

// Starts the thread and makes it bgfx's render thread. Call it before bgfx::init.
void RenderThread_Start(void);

// Call it once bgfx::init has succeeded, before the first bgfx::frame the caller counts.
void RenderThread_After_Init(void);

// Ends the thread. Call it after bgfx::shutdown, or after a bgfx::init that failed.
void RenderThread_Stop(void);

bool RenderThread_Running(void);

// Counts a frame just handed over by bgfx::frame.
void RenderThread_Note_Frame(void);

// True while the thread has not finished every counted frame, so the next bgfx::frame could wait for it.
bool RenderThread_Busy(void);
