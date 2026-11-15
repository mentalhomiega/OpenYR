---
key: BerserkROFMultiplier
summary: "Multiplies the reload time of a berserk object; smaller fires faster."
see_also: [Psychedelic, BerserkFriendly, ROF]
when_omitted:
  kind: value
  value: "0.5"
---

`BerserkROFMultiplier` is an Ares key in `[CombatDamage]`. While an object is berserk from a [`Psychedelic=yes`](/keys/psychedelic/) warhead, the delay before its next shot is its weapon's normal reload time multiplied by this value and rounded down to a whole frame. The default of `0.5` is the halved reload time Yuri's Revenge always gave berserk objects; `1.0` makes them fire at their normal rate.

```ini title="rulesmd.ini"
[CombatDamage]
BerserkROFMultiplier=1.0
```
