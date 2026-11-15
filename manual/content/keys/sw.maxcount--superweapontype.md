---
key: SW.MaxCount
scope: superweapontype
summary: "Sets how many launch structures a hunter seeker or EM pulse shot uses."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

A `Type=HunterSeeker` shot sends a seeker from this many of the house's launch structures. A `Type=EMPulse` shot fires this many cannons, nearest first. A negative value uses every one. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.MaxCount=3
```

See [Hunter seeker](/systems/superweapons/#hunter-seeker).
