# Graphics device loss

When Windows takes the graphics device away from the game, the game restarts its renderer and carries on where it
was: on a menu, in a mission, during a movie or in a network game. Switching the displays off and on, a driver update
or reset, a display mode change, sleep and resume, and a remote desktop session can all take the device away. Before
this, the game ended with the fatal error "Device is lost" (`DXGI_ERROR_DEVICE_REMOVED`).

The code is in `code/bgfxbackend.cpp` (noticing the loss, the restart) and `code/video.cpp` (when to restart, the
retries, the message).

## What the player sees

The window stops changing for as long as the device is gone, and then shows the game as it is now. Nothing is paused:
a mission or a network game goes on while the window is not updated, and the simulation runs exactly as it would with
a working display. The debug log gets one line when the loss is noticed and one when the renderer runs again:

```text
Video: the graphics device was lost (Device is lost. FAILED HRESULT(887a0005) DXGI_ERROR_DEVICE_REMOVED ...); restarting the renderer
Video: the renderer restarted after 140 ms on Direct3D 11
```

If the renderer cannot be started for 60 seconds, the game asks whether to keep waiting or to quit, in a plain
message box instead of a crash report. Keep waiting gives it another 60 seconds. An unattended test run quits with
exit code 1 instead of asking.

## How it works

bgfx reports a lost device through its fatal callback (`bgfx::Fatal::DeviceLost`) when a present fails with
`DXGI_ERROR_DEVICE_REMOVED`, `DXGI_ERROR_DEVICE_RESET` or `DXGI_ERROR_DEVICE_HUNG`. The callback records the reason
and returns, and bgfx carries on with its no-op renderer. With `RenderThread=yes` this happens on the render thread.
Every other fatal renderer error still ends the game with a crash report.

At the end of that present, and at each present after it while the device is down, the main thread:

1. releases what the interface holds on the GPU: RmlUi's textures and compiled geometry (pictures, fonts, cameos
   and the other surface elements), the developer overlay's textures, and the overlay renderer's own program,
   sampler and buffers;
2. releases what the presenter and the map passes hold (frame, layer, mask and keyed textures, the scene, mask
   and screenshot targets, the composite program, every map pass store, target and program, and the GPU voxel
   stores through `GpuVoxel_Shutdown`) and shuts bgfx down;
3. starts bgfx again on the same window, with the graphics API that ran before, the same HDR or SDR back buffer,
   the same render thread setting and the same graphics card choice, and makes the frame texture at the frame size;
4. starts the overlay renderer again, and marks the frame, the map layer and the overlay for a full present. The
   next map draw is a full redraw, so the map passes fill their stores again from the game's buffers.

Pictures, glyphs and geometry of the interface, and the voxel and map stores, are made again the next time they are
drawn, from the copies the game keeps in memory. No file is read again.

A restart asks for the API that ran and nothing else, so a failed restart never falls through to another API. While it
fails, it is tried again after 250 ms, then after waits that double up to 4 seconds. Each present call made while the
device is down waits one present interval, at least 10 ms, so a menu loop that presents does not keep a core busy.

A capture that a test asked for and that the lost device had not delivered is dropped; its step fails on its own
timeout. A frame size or window size change made while the device is down is used by the restart.

## Testing

The autotest steps `devicelost [refusals]`, `devicestate` and `minimised <0|1>` exist in game scripts and, as `ui`
steps, in menu scripts; `devicelost` and `devicestate` also run while a movie plays ([AUTOTEST.md](AUTOTEST.md)).
`devicelost` makes the next present report the loss through the same callback, with the same message a failed
Direct3D 11 present gives, and everything after that is the code a real loss runs. What the step does not reproduce:

- the device stays alive, so bgfx does not switch to its no-op renderer, and the releases free real Direct3D
  objects rather than ones of a removed device;
- a restart is refused only when the step asks for it, so the case where Direct3D cannot make a device or a swap
  chain for a while is shown by refused restarts rather than real failures;
- `minimised 1` skips presents as a minimised window does without touching the window, which stays hidden.

`tests/autotest/device-loss-game.txt` (with `device-loss-game-control.txt` for the pictures without a loss) and
`tests/autotest/device-loss-menu.txt` cover a loss in a skirmish with the modern interface, two in a row, one while
minimised, one with refused restarts, and losses on the title menu, the Settings screen and during a movie. The loading
screen is not covered, since no script step runs while it is shown.

Only real hardware shows that Direct3D gives a new device after a driver reset or with the displays switched off,
and how long that takes.
