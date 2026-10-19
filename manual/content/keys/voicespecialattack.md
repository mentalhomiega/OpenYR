---
key: VoiceSpecialAttack
summary: The responses a unit speaks for a guard, area guard, patrol or sabotage order.
see_also: [VoiceMove, VoiceAttack]
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[MYINF] ; an InfantryType registered in [InfantryTypes]
VoiceSpecialAttack=MYINF_Special1,MYINF_Special2 ; sound IDs registered in SOUND.INI
```

A unit, meaning a vehicle, infantryman or aircraft, speaks this list when the player gives it a guard, area guard, patrol or sabotage order. One entry is picked at random and played as a sound without a place, so it does not fade with the object's distance from the view; see [Placed sounds](/systems/sound-effects/#placed-sounds).

The move list does not answer these orders. With an empty list, the unit is silent for them.

The [guard key](/commands/guardobject/) gives one of these orders, and it can reach an armed structure that packs up into a vehicle. With the guard key, only the first selected object that takes the guard order can answer.
