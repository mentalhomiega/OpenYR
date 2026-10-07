---
title: List only the campaign movies the player has seen
category: feature
release: 0.2.0
targets:
- type: key
  id: NetID
  effect: added
- type: system
  id: movies-and-credits
  effect: changed
credit:
- MentalHomiega
---

Play Movies now lists the intro movie and then only the campaign movies up to the latest one the player has seen, as Yuri's Revenge does, where it listed every movie before. The movies seen are stored in the [`NetID=`](/keys/netid/) entry of `RA2MD.INI` in Yuri's Revenge's format, so the progress of an existing settings file is kept. A settings file without the entry lists only the intro movie.
