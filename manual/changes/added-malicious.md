---
title: Let a warhead hit without raising the harvester or base alert (Malicious)
category: feature
release: 0.2.0
targets:
- type: key
  id: Malicious
  effect: added
credit: [MentalHomiega]
---

A warhead with `Malicious=no` no longer raises the harvester or base under attack alert for a player when it hits a harvester or an undeployable gatherer structure, as Ares does. Damage is unchanged. Warheads that leave the key out are unchanged.
