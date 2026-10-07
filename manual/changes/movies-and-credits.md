---
title: Play Yuri's Revenge's movies and credits
category: feature
release: 0.2.0
targets:
- type: system
  id: movies-and-credits
  effect: added
- type: format
  id: bink
  effect: added
- type: format
  id: vqa
  effect: changed
credit: [MentalHomiega]
---

We now play Yuri's Revenge's `.BIK` movies through the `BINKW32.DLL` of the player's install, and a `.VQA` movie name with no such file plays the `.BIK` of the same name. Movies & Credits uses them. Sneak Peeks plays the Renegade trailer, Play Movies lists the campaign movies, and View Credits scrolls `CREDITSMD.TXT`, with its string table labels filled in, over the `CREDITS` music. The Play Movies item in the classic menu style is no longer dimmed. Without the DLL, Bink movies are skipped.
