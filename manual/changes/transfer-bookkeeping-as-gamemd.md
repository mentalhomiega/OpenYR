---
title: Transfer structures and trigger hand-overs as gamemd does
category: fix
release: 0.2.0
targets:
- type: system
  id: mind-control
  effect: changed
- type: system
  id: capture
  effect: changed
credit: [MentalHomiega]
---

A structure taken over by mind control, a psychic dominator, a team, or a trigger now also moves its construction yard, factory, and build-time records to the new owner, as a captured structure does. Before, those records stayed with the losing house, so it could still count a factory it had lost, and the build time of its other factories used that count. The Change House and All change house actions and a slave miner freed by its master's death no longer spring the object's entered trigger. A radio-linked object, such as an aircraft on a helipad, now changes owner with its structure on every path.
