---
title: Offer the player colors that the UI file lists
category: feature
release: 0.2.0
targets:
- type: key
  id: Count
  effect: added
- type: format
  id: ui-ini
  effect: changed
- type: system
  id: skirmish-setup
  effect: changed
credit:
- MentalHomiega
---

The skirmish and network lobbies, the color picker and the game now follow the `[Colors]` section of the UI file: `Count=` sets how many colors are offered, up to 16, and `SlotN.ColorScheme=`, `SlotN.DisplayColor=`, `SlotN.Tooltip=` and `Observer.ColorScheme=` set each slot, as Ares does. Mental Omega's 14 colors now appear in the lobby, and a player who picks one is drawn in that slot's scheme, where the engine used the stock scheme positions and so showed other colors. Without the section the lobbies offer the eight stock colors as before.
