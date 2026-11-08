---
title: Read a modifier-only key for the held commands under any trigger
category: fix
release: 0.2.0
targets:
- type: command
  id: ForceFire
  effect: changed
- type: command
  id: ForceMove
  effect: changed
- type: command
  id: SelectModifier
  effect: changed
- type: command
  id: QueueMove
  effect: changed
- type: command
  id: GridBuildModifier
  effect: changed
credit: [MentalHomiega]
---

Shift, Control or Alt alone, or a combination such as Control with Alt, now works for Force fire, Force move, Select modifier, Queue move and Grid build modifier under any trigger, including the Press that a new slot starts with. Before, the controls page accepted such a binding, but the game read it only when its trigger was Held, so it did nothing. A keyboard file that already holds such a binding starts working, and the file itself does not change.
