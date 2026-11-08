---
title: Search, sort and filter the multiplayer map list
category: feature
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The Select Multiplayer Map screen, which the skirmish lobby, the LAN lobby and the CnCNet room share, lists the maps by name, with numbers in order ("Map 2" before "Map 10"), and shows how many start positions each map has. A search box above the list filters as you type; every word must appear in the map's name or file name, ignoring case. Players: shows only maps with a chosen number of start positions, and Sort by: orders the list by name or by player count. A line under the list gives the number of maps shown, and the picture, player count and file name of the highlighted map sit beside it.

The search box has the keyboard focus when the screen opens. Up, Down, Page Up and Page Down move the highlight from the box, Enter picks the highlighted map, and Esc cancels. A printable key pressed with the focus elsewhere goes into the search box. Double-clicking a map picks it.
