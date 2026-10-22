---
key: Renderer
summary: Which graphics interface the game draws through, as a number.
when_omitted:
  kind: value
  value: "0"
  note: Zero lets the game choose the interface.
---

The number selects the graphics interface the game asks for when it starts. It exists to work around a driver problem, so leave it at `0` unless a driver needs a specific interface.

| Value | Interface |
| --- | --- |
| `0` | Chosen automatically |
| `1` | Direct3D 11 |
| `2` | Direct3D 12 |
| `3` | Vulkan (experimental) |
| `4` | OpenGL |

A value outside this range is treated as `0`.

Avoid `3` for normal play. On one NVIDIA system the Vulkan interface stopped responding when the in-game Options menu opened, and the whole computer froze.

The [debug log](/using/debug-logging/) names the interface that started. Check it to confirm that a requested interface is in use.

If the game cannot start drawing, it reports that it is unable to set the video mode and closes. Set `Renderer` back to `0` in the settings file to undo a value that causes this.

The **Graphics API** list on the Display page of the settings stores a new value for the next launch; the game keeps the interface it started with.
