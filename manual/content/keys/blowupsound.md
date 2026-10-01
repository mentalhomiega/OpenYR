---
key: BlowupSound
summary: "Parsed structure damage sound that Yuri's Revenge replaced with BuildingDamageSound."
see_also: [BuildingDamageSound, DamageSound, CrumbleSound]
no_effect: true
when_omitted:
  kind: value
  value: none
---

The value is read but changes nothing. A structure that a hit takes below half strength or into the red plays [`BuildingDamageSound`](/keys/buildingdamagesound/), or nothing when its type sets its own [`DamageSound`](/keys/damagesound/).
