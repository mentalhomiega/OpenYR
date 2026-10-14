---
title: Show four rows of build cameos in the modern panel
category: feature
release: 0.2.0
targets:
- type: key
  id: SidebarRows
  effect: added
credit: [MentalHomiega]
---

The modern build panel used to run down the whole screen height. It now shows four rows of cameos by default, as OpenRA's sidebar does, and ends under its scroll arrows, so the map shows below it. [`SidebarRows`](/keys/sidebarrows/) sets the number of rows, and the Display page previews a new number in a game before OK. Each scroll arrow now moves the panel by a screenful of rows, as the classic arrows do.
