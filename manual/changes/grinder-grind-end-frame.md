---
title: End a grinding animation at its healthy frames
category: fix
release: 0.2.0
targets:
- type: key
  id: LoopEnd
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

A shadow animation that plays once now ends at its `LoopEnd`, as Yuri's Revenge does. A Yuri Grinder's grinding animation (`YAGRND_B`) runs to frame 48 and stops there, so a healthy grinder no longer shows the damaged frames that follow. Before, the animation ran on to the end of its frame count, through about 48 more frames of the damaged art.
