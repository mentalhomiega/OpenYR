---
title: Leave crates where CrateBeneath structures stood
category: feature
release: 0.2.0
targets:
- type: key
  id: CrateBeneath
  effect: added
- type: key
  id: CrateBeneathIsMoney
  effect: added
- type: system
  id: crates
  effect: changed
credit: [MentalHomiega]
---

Destroyed structures with `CrateBeneath=yes`, such as the Washington monuments, now leave a crate, as in Yuri's Revenge. Crates dropped by destroyed vehicles now give a random result in campaigns too, and a crate drawn into a multiplayer map gives money unless its overlay data marks it random.
