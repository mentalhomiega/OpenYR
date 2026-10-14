---
title: Match the classic sidebar's scroll arrows, tabs, sounds and tooltips to gamemd
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
- type: key
  id: ScoldSound
  effect: changed
credit:
- MentalHomiega
---

In the classic sidebar, each scroll arrow moves the shown strip a screenful, on either mouse button, as in Yuri's Revenge. Before, the left arrow moved one row. A click acts when the button is released, and holding the button does not repeat it.

The two arrows are enabled only when the shown tab holds more cameos than fit on the screen. Otherwise they are disabled and ignore clicks.

The first cameo added to an empty panel now shows its tab. When a recheck empties a strip, the shown tab moves to the first tab that still holds cameos.

Clicks on the repair and sell buttons play `GUIMainButtonSound`. Clicks on a tab, and on an arrow that moves the strip, play `GUITabSound`. Both keys are read from `[AudioVisual]`; the stock rules set them to `MenuClick` and `MenuTab`. A refused arrow click still plays `ScoldSound`.

Tabs and scroll arrows show tooltips, and a disabled one adds `(Disabled)` on a second line. The modern panel is unchanged.
