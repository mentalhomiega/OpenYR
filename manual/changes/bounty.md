---
title: Pay bounty for destroyed objects
category: feature
release: 0.2.0
targets:
- type: system
  id: bounty
  effect: added
- type: key
  id: Bounty
  effect: added
- type: key
  id: Bounty.Value
  effect: added
- type: key
  id: Bounty.RookieValue
  effect: added
- type: key
  id: Bounty.VeteranValue
  effect: added
- type: key
  id: Bounty.EliteValue
  effect: added
- type: key
  id: Bounty.Display
  effect: added
- type: key
  id: BountyDisplay
  effect: added
- type: key
  id: BountyEnablers
  effect: added
- type: key
  id: GivesBounty
  effect: added
credit:
- MentalHomiega
---

An object with `Bounty=yes` in its rulesmd.ini section earns its owner the destroyed object's `Bounty.Value` credits, or its `Bounty.RookieValue`, `Bounty.VeteranValue` or `Bounty.EliteValue` for that rank. A negative value takes credits from the owner instead. `GivesBounty=no` on a country and `BountyEnablers` in `[General]` limit who pays and who collects, and `Bounty.Display`, or `BountyDisplay` in `[AudioVisual]`, shows each payment over the destroyed object. The keys follow the Ares documentation.
