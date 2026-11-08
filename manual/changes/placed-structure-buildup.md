---
title: Play the construction buildup of a structure placed during play
category: fix
release: 0.2.0
targets:
- type: key
  id: Buildup
  effect: changed
credit:
- MentalHomiega
---

A structure placed during play now plays its [`Buildup=`](/keys/buildup/) construction animation before it opens. Some structures opened on the placement frame and skipped the animation, such as the Yuri Bio Reactor (`YAPOWR`) and the Yuri Grinder (`YAGRND`). Structures that already played their buildup, such as `GAPOWR` and `NAPSIS`, are unchanged.
