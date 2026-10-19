---
key: HunterSeeker.Buildings
scope: superweapontype
summary: "Lists the structures that launch a hunter seeker."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[SpecialWeapons] HSBuilding"
---

A shot sends a seeker from the house's structures of these types, up to [`SW.MaxCount=`](/keys/sw.maxcount/) of them, taking the last built first. A house with none launches nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
HunterSeeker.Buildings=GAHPAD
```

See [Hunter seeker](/systems/superweapons/#hunter-seeker).
