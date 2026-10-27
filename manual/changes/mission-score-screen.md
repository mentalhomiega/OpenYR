---
title: Show the mission score screen with Yuri's Revenge's layout
category: feature
release: 0.2.0
targets:
- type: key
  id: SkipScore
  effect: changed
credit:
- MentalHomiega
---

The screen after a won campaign mission is Yuri's Revenge's Mission Score screen. It shows the rank and the commendation for the mission, the par time and the player's time, and the enemies the player destroyed. The rank is the under-par title when the player's time is not more than the par time for the difficulty, and the over-par title otherwise. The titles and commendations come from the `[Ranking]` section of the mission map: `ParTimeEasy`, `ParTimeMedium`, `ParTimeHard`, `UnderParTitle`, `UnderParMessage`, `OverParTitle` and `OverParMessage`. A mission that leaves them out shows the original's placeholder text. Only its Continue button closes the screen.

The enemies destroyed are the units, aircraft, infantry and structures that the player's houses killed, except types with `DontScore=yes`.

With the Classic interface style, the screen draws the original 800 by 600 layout: the side's plate and emblem, the framed rank and commendation, the two time boxes, the metal column with its monitor and the Continue button, and the status bar. The emblem plays its 44 frames at 100 milliseconds each and then repeats frames 29 to 43, as the original does. On a larger screen, the area around the layout shows a dimmed, blurred copy of it. The text is yellow for every side.

With the Modern interface style, the screen has three panels across the window: the rank and commendation, the emblem at the width of its panel, and the figures with Continue.

The commendation appears whole rather than typed out.
