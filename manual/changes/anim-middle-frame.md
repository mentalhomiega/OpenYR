---
title: Lay craters, scorch marks and fires on the middle frame
category: fix
release: 0.2.0
targets:
- type: key
  id: Crater
  scope: animtype
  effect: changed
- type: key
  id: Scorch
  effect: changed
- type: key
  id: Flamer
  effect: changed
- type: key
  id: SpawnsParticle
  scope: animtype
  effect: changed
- type: key
  id: NumParticles
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

An animation's craters, scorch marks, fires and particles now come on its middle frame, the frame whose index is half its frame count. Before, they came on the frame with the largest artwork, and the size of a crater or scorch mark came from that frame too. A [`Theater=yes`](/keys/theater/#scope-animtype) animation with no `Image=` key has no middle frame, so its effects come as it starts and its marks are single-cell.
