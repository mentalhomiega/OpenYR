---
title: Open at the desktop resolution when no size is set
category: feature
release: 0.2.0
targets:
- type: key
  id: ScreenWidth
  effect: changed
- type: key
  id: ScreenHeight
  effect: changed
credit:
- MentalHomiega
---

A [`ScreenWidth`](/keys/screenwidth/) or [`ScreenHeight`](/keys/screenheight/) under `[Video]` in `RA2MD.INI` that is missing or `-1` now takes the primary display's desktop size in physical pixels, instead of 640 by 480. A display at 150 percent scaling therefore opens at its full pixel count. A size the file gives stays as written. To keep the old screen, set both keys to `640` and `480`.
