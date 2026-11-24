---
title: Check an aircraft's heading after its other fire restrictions
category: fix
release: 0.2.0
targets:
- type: system
  id: aircraft-operations
  effect: changed
- type: key
  id: Fighter
  effect: changed
credit: [MentalHomiega]
---

An aircraft that is not a fighter reports why it cannot fire before it reports a wrong heading, so a target out of range or a reload shows first. A fighter no longer waits to be turned toward its target before it fires, and an ion storm stops only the aircraft whose weapon is ion sensitive.
