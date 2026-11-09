# Render thread

`RenderThread=yes` in the `[Video]` section of `RA2MD.INI` moves Direct3D 11 submission and the present to a thread of their own. The default is `no`, and with it the game draws and presents on the main thread as before. The key is read at startup, has no row in the settings, and is ignored unless `Renderer` is automatic or Direct3D 11.

The thread is in `code/renderthread.cpp`. The key is declared in `code/optionsettings.cpp` with `.BeforeRenderer`, because the renderer starts before the other settings are read.

## What runs where

The main thread still runs the simulation, draws the map, records the GPU pass and records every bgfx call. The render thread runs `bgfx::renderFrame`, which makes the Direct3D 11 calls and the present. It reads no game state, draws no random numbers and sets no timing that the simulation reads, so saves, replays and network sync are unchanged.

| Data | Owner | How it crosses |
|---|---|---|
| Game objects, surfaces, depth and alpha buffers | Main thread | They do not cross. Their contents cross as the copies below. |
| Texture uploads, vertex data, uniforms | bgfx | Copied into bgfx memory when recorded, and freed by bgfx after the render thread has used them. |
| Depth and alpha buffers of a persistent pass | Main thread | Copied when the thread is on. Without the thread the first pass hands bgfx a reference, because the frame is finished before the buffers change. |
| The record of a frame | bgfx | bgfx keeps two records. `bgfx::frame` swaps them, so the main thread writes one while the render thread reads the other. |
| Capture and `gpucheck` pixels | The caller's buffer | The render thread writes it, and the main thread reads it once `bgfx::frame` has returned the frame number `bgfx::readTexture` gave. |
| Frames handed over and finished | `renderthread.cpp` | The main thread counts frames handed over; the render thread counts frames finished, in an atomic. |

## Handing over a frame

`Backend_End_Frame` calls `bgfx::frame`, which waits only until the render thread has finished the frame before it. `Video_Present_If_Dirty` does not start a present while the render thread has not finished every frame handed to it, so that wait does not happen. The present is put off, its changes stay marked, and the next one shows the newest picture. The `presents busy` count in the Perf lines counts these.

`Video_Present` and `Video_Present_Now`, which screens outside the battle use, do not check and can wait.

## Frame rate limit, RenderFrameRate and RenderBlend

The frame rate limit is checked on the main thread before the busy test, as it is without the thread. `RenderFrameRate` and `RenderBlend` belong to the main thread's redraw loop and are unchanged. A redraw costs the main thread less, so fewer are skipped.

## Other paths

| Path | Behaviour |
|---|---|
| Shutdown | `bgfx::shutdown` finishes the frames in hand, then the thread ends. A renderer that fails to start ends its thread the same way, so the SDR retry after an HDR failure starts a new one. |
| Resize | `bgfx::reset` and the rebuilt targets are recorded like any other call and take effect with the next frame. |
| Device loss | The bgfx callback raises the fatal error on the render thread. The crash filter covers every thread, so the report and exit are the same. |
| Screenshots | A `windowshot` is written by the render thread after the frame it was requested in, so the file can appear after the step has returned. Under HDR the game writes the file from the read-back pixels on the main thread. |
| `framedump` | The game has no such path. |

## Measurements

These runs are at 3840x2160 with `GpuRender=yes`, `RenderFrameRate=240`, `FrameRateLimit=0` and HDR output on, playing `bench-large-battle.txt` and `bench-late-game.txt`. They are alternating runs on one build, two of each setting. VSync is off except in the last row group, which is one run of each on the large battle. Times are milliseconds on the main thread, as mean, p95 and worst, and the two runs are listed in order.

| Large battle | Off | On |
|---|---|---|
| Draws per second | 300, 300 | 300, 300 |
| `draw.total` mean | 0.815, 0.814 | 0.625, 0.612 |
| `draw.total` p95 | 1.73, 1.51 | 1.20, 1.12 |
| `draw.total` worst | 11.2, 9.9 | 5.9, 6.9 |
| `present.gpu` mean | 0.190, 0.199 | 0.000, 0.000 |
| `present.gpu` worst | 5.3, 8.4 | 0.2, 0.7 |
| Worst game frame interval | 19.6, 22.5 | 18.8, 37.5 |

| Late game | Off | On |
|---|---|---|
| Draws per second | 228.5, 228.0 | 235.7, 236.4 |
| `draw.total` mean | 1.398, 1.410 | 1.160, 1.149 |
| `draw.total` p95 | 4.56, 4.35 | 3.72, 3.51 |
| `draw.total` worst | 14.2, 13.9 | 10.1, 10.0 |
| `present.gpu` mean | 0.242, 0.246 | 0.000, 0.000 |
| `present.gpu` worst | 5.1, 5.1 | 0.7, 1.1 |
| `frame.logic` mean | 4.41, 4.40 | 4.48, 4.46 |
| Draws skipped per second | 22, 22 | 16, 16 |

| Large battle, VSync on | Off | On |
|---|---|---|
| Draws per second | 238.6 | 300 |
| Presents per second | 238.6 | 237.1 |
| `draw.total` mean | 3.41 | 0.46 |
| `present.gpu` mean | 2.76 | 0.00 |
| Draws skipped per second | 52 | 0 |
| p95 of the per-second worst game frame interval | 20.65 | 16.76 |

The worst interval of 37.5 ms is one second of one run. In another second of that run the present stalled for 156 ms and 92 presents were put off, while no game frame interval in the seconds around it exceeded 16.7 ms.

## Limits

- Only Direct3D 11 has been run with the thread.
- `present.gputime` reads bgfx's statistics while the render thread may write them, so one sample can be wrong. It feeds only the Perf lines.
- The hang watchdog watches the main thread, so a driver hang on the render thread shows as a frozen picture while the game runs on, until a `Video_Present` or `Video_Present_Now` waits for it.
- Fullscreen changes have not been stress-tested. Resizing the window has: 22 `windowresize` steps during a battle ended normally.
