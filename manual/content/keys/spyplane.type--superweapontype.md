---
key: SpyPlane.Type
scope: superweapontype
summary: "Names the aircraft a spy plane weapon sends."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "SPYP, one for each AllyParaDropInf entry"
---

An aircraft type. When the key is set, the weapon sends [`SpyPlane.Count=`](/keys/spyplane.count/) of it on the spy plane approach mission. Without it, the weapon sends one `SPYP` for each [`AllyParaDropInf`](/keys/allyparadropinf/) entry.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SpyPlane.Type=SPYP
```

See [Spy plane](/systems/superweapons/#spy-plane).
