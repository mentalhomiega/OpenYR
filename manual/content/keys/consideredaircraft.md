---
key: ConsideredAircraft
summary: Parsed flag for whether an object counts as aircraft for the air threat scan. The engine does not use it.
no_effect: true
when_omitted:
  kind: context-dependent
  note: "Yes for an aircraft type. No for a vehicle, infantry or structure type."
---

The engine reads this key and does not use it. Its air threat scan takes each aircraft, jumpjet infantryman and vehicle that is in the air, whatever the value.
