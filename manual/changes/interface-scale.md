---
title: Draw the in-game interface larger on tall screens
category: feature
release: 0.2.0
targets:
- type: key
  id: InterfaceScale
  effect: added
credit: [MentalHomiega]
---

At a screen height of 1620 lines or more, the sidebar, command bar, tooltips and messages used to be drawn at their original size and looked tiny at 3840 by 2160. We now draw them [`InterfaceScale`](/keys/interfacescale/) times larger, which is 2 at 3840 by 2160 unless the setting says otherwise, while the map is still drawn at the screen's own resolution. Screens of 1080 lines or less look as they did.
