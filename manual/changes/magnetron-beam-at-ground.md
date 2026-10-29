---
title: Draw the magnetron beam at bare ground
category: fix
release: 0.2.0
targets:
- type: key
  id: IsMagBeam
  effect: changed
credit:
- MentalHomiega
---

A magnetron ordered to fire at a cell with nothing on it now draws its beam, as in Yuri's Revenge. Before, the beam was drawn only at an object. The beam also ends when the firer stops aiming at that cell.
