---
title: Announcer queue follows Yuri's Revenge
category: fix
release: 0.2.0
targets:
- type: system
  id: eva-speech
  effect: changed
credit:
- MentalHomiega
---

The announcer now queues and starts lines as Yuri's Revenge does. A line starts at once when the announcer is free, instead of after a second. Waiting lines play interrupt lines first, then critical lines, then the one held `STANDARD` line, then `QUEUE` lines by priority, with no limit on how many wait. Play speech lines cut the line speaking, so a mission's dialogue is no longer lost behind routine announcements. Lines asked for at zero voice volume take their turn without sound, and the chronoshift screen effect and movies hold the announcer as in the original game.
