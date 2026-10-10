---
title: Draw the 640 campaign loading picture in its side's palette
category: fix
release: 0.2.0
targets:
- type: format
  id: missionmd-ini
  effect: changed
credit:
- MentalHomiega
---

On a screen 640 pixels wide, a campaign mission's loading picture now uses the palette of its side, `LDSCRNA.PAL` for the first side and `LDSCRNS.PAL` for the other. Before, it used the `LS800BkgdPal=` palette, which made its colours wrong. The 800 by 600 picture is unchanged.
