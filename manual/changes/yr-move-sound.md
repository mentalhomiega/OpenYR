---
title: Play MoveSound while an object moves
category: fix
release: 0.2.0
targets:
- type: key
  id: MoveSound
  effect: added
credit: [MentalHomiega]
---

`MoveSound` in rulesmd.ini was ignored. A sound picked from the list now plays while an object of the type moves, and stops a few frames after it halts or when it leaves the map.
