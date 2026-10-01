---
key: FireOnce
summary: "Makes the firer drop its target after one shot of this weapon."
see_also: [MakesDisguise, IvanBomb]
when_omitted:
  kind: value
  value: "no"
---

After a shot with this weapon, the firer drops its target. It may pick a target again on its own afterwards.

```ini title="rulesmd.ini"
[MyMakeupKit] ; example Weapon
FireOnce=yes
```
