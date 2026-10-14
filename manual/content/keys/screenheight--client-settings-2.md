---
key: ScreenHeight
scope: client-settings-2
label: Height the display opens at
see_also: [ScreenWidth, WindowHeight, Fullscreen]
when_omitted:
  kind: computed
  note: A height left out keeps the launch option's height, if any. A height still unset, or written as `-1`, becomes the primary display's desktop height.
---

The game reads this height with [`ScreenWidth`](/keys/screenwidth/#scope-client-settings-2) before it opens its window, and renders at the resulting size. That page covers how the pair combines with the launch option, the desktop size and its 640 by 480 fallback, and what happens when the renderer cannot start.

The height is the full height of the game screen. [The sidebar](/systems/sidebar/) runs down the whole of it, and the tactical view takes the height left below a 16-pixel strip across the top.
