---
title: Show power plants going dark in a blackout or a drain
category: feature
release: 0.2.0
targets:
- type: key
  id: PoweredSpecial
  effect: added
- type: key
  id: LowPower
  effect: changed
credit: [MentalHomiega]
---

`PoweredSpecial=` is now read from a structure's rules section, as is each animation slot's `PoweredSpecial=`. A power plant with it set to `yes` goes out of service in a spy's power blackout and while it is drained. Its marked animations then stop and its `LowPower` animation plays, as the Tesla reactor's does, and both are reversed when the plant works again.

Structures that need power now change their animations only when they drop out of or come back into service, rather than each time their house recounts its power. A `Powered=yes` animation begun while the structure has no power now starts frozen. Saved games made by earlier builds are refused, because a type now stores the setting.
