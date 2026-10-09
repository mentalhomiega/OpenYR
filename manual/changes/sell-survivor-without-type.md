---
title: Sell a crewed structure past a survivor with no type
category: fix
release: 0.2.0
targets:
- type: key
  id: Technician
  effect: changed
credit: [MentalHomiega]
---

A sold structure whose next survivor has no type now leaves that one survivor out and releases the rest, as a destroyed structure always did. It used to stop releasing survivors at that point. A survivor has no type when its crew key, such as `AlliedCrew=`, or [`Technician=`](/keys/technician/) is `none`.
