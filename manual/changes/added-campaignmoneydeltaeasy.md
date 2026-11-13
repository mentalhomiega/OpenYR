---
title: Start campaign houses with a money delta by difficulty (CampaignMoneyDeltaEasy)
category: feature
release: 0.2.0
targets:
- type: key
  id: CampaignMoneyDeltaEasy
  effect: added
credit: [MentalHomiega]
---

A campaign house with `PlayerControl=yes` now starts a mission with its credits plus `CampaignMoneyDeltaEasy` when the player plays on Easy, never below zero, as Yuri's Revenge does. The stock values are 5000 and -5000, so missions whose rules leave the keys out start with other money on Easy and Hard than before. Normal is unchanged.
