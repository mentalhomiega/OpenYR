---
title: Read the house numbers of Red Alert 2 missions as Red Alert 2 numbers them
category: fix
release: 0.2.0
targets:
- type: format
  id: opents-ini
  effect: changed
credit:
- MentalHomiega
---

A mission of a campaign whose `Scenario=` has the `RA2->` prefix now reads the house numbers of its trigger events, trigger actions and team scripts against Red Alert 2's `[Countries]` list. Before, they were read against Yuri's Revenge's list, which has `YuriCountry` inserted at 9, so every number from 9 up named the wrong country. The effects were visible in Boot Camp day 1, where the four soldiers of the shooting range stayed hostile and shot the player's soldier, and in missions whose win or loss trigger named a house that no one plays, so they ended at once.

`RA2Rules=` under `[Files]` in `OPENTS.INI` names the rules file whose list is used. Without that file the numbers are read as before.
