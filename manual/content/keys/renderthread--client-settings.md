---
key: RenderThread
scope: client-settings
label: Render thread
when_omitted:
  kind: value
  value: "no"
---

`RenderThread=yes` runs the graphics device on a thread of its own, so a slow draw or present does not hold up the game. The key is read from the `[Video]` section of `RA2MD.INI` at startup, and the game writes it back when it saves its settings. The Performance page of the settings sets it with the switch **Render on a separate thread**; a change applies at the next start.

It takes effect only with `Renderer` at its default or Direct3D 11. Other renderers ignore it.

While the thread is still finishing the last picture, the game puts the next picture off and draws a newer one instead.
