---
title: Keep campaign triggers whose owner nobody plays
category: fix
release: 0.2.0
targets:
- type: system
  id: trigger-springing
  effect: changed
credit:
- MentalHomiega
---

A campaign trigger owned by a country that no house plays now stays in the mission and runs, as in Yuri's Revenge. Soviet missions have many triggers owned by `Americans` (Soviet 3 has 68), and the Allied missions have a few owned by `YuriCountry2` or by `Americans` where the player's country only has them as a parent. They were thrown away when the map was read, so their briefings, objective reminders, reinforcements and chained triggers never happened. Such a trigger has no house to act for, so an action that works on the owner's house, such as an alliance or a super weapon charge, does nothing from it. Skirmish and multiplayer maps still drop a trigger whose owner is not playing.
