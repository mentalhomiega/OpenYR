---
title: Let a side name its score screen art and music
category: feature
release: 0.2.0
targets:
- type: key
  id: MultiplayerScore.Background
  effect: added
- type: key
  id: MultiplayerScore.Palette
  effect: added
- type: key
  id: MultiplayerScore.Bars
  effect: added
- type: key
  id: MultiplayerScore.WinTheme
  effect: added
- type: key
  id: MultiplayerScore.LoseTheme
  effect: added
- type: key
  id: CampaignScore.Background
  effect: added
- type: key
  id: CampaignScore.Animation
  effect: added
- type: key
  id: CampaignScore.Palette
  effect: added
- type: system
  id: multiplayer-score-screen
  effect: changed
credit:
- MentalHomiega
---

A side's section can replace the picture, palette and bands of the multiplayer score screen, the art of the campaign score screen, and the theme the multiplayer screen plays after a win or a loss. A side that sets none keeps its stock art and the `SCORE` theme. The key names follow Ares.
