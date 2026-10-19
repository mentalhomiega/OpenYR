---
key: ParaDrop.Aircraft
scope: superweapontype
summary: "Names the aircraft that carries the paratroopers."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "PDPLANE"
---

An aircraft type, used with [`ParaDrop.Types=`](/keys/paradrop.types/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ParaDrop.Aircraft=PDPLANE
```

See [Paradrops](/systems/superweapons/#paradrops).
