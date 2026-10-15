---
title: Set the window size from the display page
category: feature
release: 0.2.0
targets:
- type: key
  id: WindowWidth
  effect: changed
- type: key
  id: WindowHeight
  effect: changed
credit:
- MentalHomiega
---

The Display page has a Window size list that sets [`WindowWidth`](/keys/windowwidth/) and [`WindowHeight`](/keys/windowheight/). Match resolution leaves both at -1. The list offers the common sizes that fit the desktop. A window larger than the display's usable area is now shrunk to fit inside it, so a windowed game that follows the desktop resolution no longer opens off the screen.
