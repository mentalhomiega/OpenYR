---
title: Count a moving jumpjet's shot against its target's estimate
category: fix
release: 0.2.0
targets:
- type: key
  id: JumpJet
  effect: changed
- type: key
  id: JumpJet
  effect: added
  scope: aircrafttype
- type: key
  id: JumpJet
  effect: removed
  scope: infantrytype
credit:
- MentalHomiega
---

A moving firer's shot is inaccurate, and an inaccurate shot takes no estimated damage off its target. A firer whose type sets `JumpJet=yes` is now exempt from that rule, so its shots fired while it moves take the estimate, as they do when it stands still. gamemd reads the flag on the type (`TechnoTypeClass::JumpJet`, 0xD94) in `TechnoClass::Fire` (0x6FDD50). The flag now belongs to every type, where it used to belong only to infantry. Its other effects in gamemd are not part of this change.
