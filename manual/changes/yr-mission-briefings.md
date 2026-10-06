---
title: Show the Yuri's Revenge mission briefings
category: fix
release: 0.2.0
targets:
- type: key
  id: Brief
  effect: changed
credit:
- MentalHomiega
---

The objectives screen, both when a campaign mission opens and from Restate Briefing during play, now shows the string table text that `MISSIONMD.INI` names for the mission. It used to show an empty page for every stock campaign mission. A mission that `MISSIONMD.INI` does not list still shows its map's or `MISSION.INI` briefing, and otherwise, when `MISSIONMD.INI` exists, the text of `Brief:Error`. Saved games are unchanged, and games saved earlier show the briefing too.
