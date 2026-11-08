---
title: Make the CnCNet room and game list say what they mean
category: feature
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The room screen of the built-in CnCNet lobby now shows the room's name and channel above the player list, and the player list has column headings. Each player's side, color, status (Host, Ready or Not ready) and team and start are written out, so none of them depends on a picture or a name's color. The match's network options are one line above the chat, which now has a label.

The leave button says Close room for the host and Leave room for a guest. Kick Player is off until you pick a player other than yourself. A line beside the buttons says why Go! or Accept is off, such as which players are not ready, or what happens next. Wrapped chat lines no longer start with a stray space.

In the game list, a game or map name that does not fit ends in an ellipsis, and hovering over a row shows the whole name, map and player count. The list is wider.

The map picker lists each map file once. Two different files that share a name, such as two maps called The Alamo, show their file names after it.
