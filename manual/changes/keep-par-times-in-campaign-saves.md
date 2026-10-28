---
title: Keep the par times and rank texts in a campaign save
category: fix
release: 0.2.0
targets:
- type: system
  id: campaign-progression
  effect: changed
breaking: false
credit:
- MentalHomiega
---

A campaign save now keeps the mission's `ParTime` values and its four rank texts from `[Ranking]`. Before, a loaded save forgot them, so the score screen compared the mission time with a one-minute par time and showed the placeholder rank texts. Saves made by earlier builds are refused, because the save layout changed.
