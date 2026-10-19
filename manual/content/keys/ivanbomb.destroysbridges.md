---
key: IvanBomb.DestroysBridges
summary: "Whether a bomb this weapon plants on a bridge repair hut destroys the bridge."
see_also: [IvanBomb, "system:ivan-bombs", BridgeRepairHut]
when_omitted:
  kind: value
  value: "yes"
---

A bomb that goes off on a [`BridgeRepairHut=yes`](/keys/bridgerepairhut/) structure destroys the bridge beside it, whatever its damage. With `no` the structure takes the explosion and the bridge is left standing.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.DestroysBridges=no
```
