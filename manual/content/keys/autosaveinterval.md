---
key: AutoSaveInterval
summary: How many frames pass between one automatic save and the next in a game started from the menu, or zero for none.
see_also: [GameSpeed]
when_omitted:
  kind: value
  value: "10800"
---

A single player mission or skirmish started from the menu saves itself each time this many frames have passed. `0` or a negative value turns automatic saves off.

The interval counts frames, so a faster [`GameSpeed`](/keys/gamespeed/) saves more often by the clock. At `GameSpeed=3`, which runs a campaign mission or skirmish at 30 frames a second, 10800 frames is six minutes. At `GameSpeed=2`, which runs at 45, it is four. At `GameSpeed=0` the frame rate has no limit, so the time between saves depends on the computer.

[Automatic saves](/formats/save-games/#automatic-saves) describes the message each save posts, the files the saves rotate through, and what starts the count over.

A multiplayer game arranged from the menu makes no timed saves, whatever this value. A game started by a [launch file](/formats/spawn-ini/#automatic-saves) uses the file's interval instead, and a file that names none turns automatic saves off.

The **Autosave** list on the Game page of the settings offers Off, 5400, 10800 and 21600 frames, and lists any other value the file holds under its own number. A change made during a game starts the count over from that frame. In a game started by a launch file, the change is stored for later games and the file's interval stays in use. The game reads the value from `RA2MD.INI` at startup and writes it back with the other options.
