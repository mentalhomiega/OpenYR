---
title: Show the mission score screen with Yuri's Revenge's layout
category: feature
release: 0.2.0
targets:
- type: key
  id: ScoreEmblemTicks
  effect: added
credit:
- MentalHomiega
---

The screen after a won campaign mission is Yuri's Revenge's Mission Score screen. It shows the rank and the commendation for the mission, the par time and the player's time, and the enemies the player destroyed. The rank is the under-par title when the player's time is not more than the par time for the difficulty, and the over-par title otherwise. The titles and commendations come from the `[Ranking]` section of the mission map: `ParTimeEasy`, `ParTimeMedium`, `ParTimeHard`, `UnderParTitle`, `UnderParMessage`, `OverParTitle` and `OverParMessage`. A mission that leaves them out shows the original's placeholder text. The screen plays the Mission Score music and animates the side's emblem. Only its Continue button closes it.

With the Modern interface style, the screen has three panels across the window: the rank and commendation, the emblem, and the figures with Continue. With the Classic interface style, it draws the original 800 by 600 layout, and the area around it shows a dimmed, blurred copy of the art.

`ScoreEmblemTicks` in `[General]` of the rules sets how many 16 millisecond wait loops each frame of the emblem animation lasts. The original's rate is not confirmed, so the default of 3 is a setting. The original's metal column and monitor on the right of the Classic screen are not drawn yet, and the commendation appears whole rather than typed out.
