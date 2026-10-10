---
key: ClearAllWeapons
summary: Removes the primary and secondary weapons, and their elite versions, after the object's weapon keys are read.
see_also: ["Primary", "Secondary", "ElitePrimary", "EliteSecondary", "Weapon1", "WeaponCount"]
when_omitted:
  kind: value
  value: "no"
---

`ClearAllWeapons=yes` empties the first two weapon slots of the object, which hold its primary and secondary weapons, and the first two elite slots. The engine clears them after it reads [`Primary`](/keys/primary/), [`Secondary`](/keys/secondary/), [`ElitePrimary`](/keys/eliteprimary/), and [`EliteSecondary`](/keys/elitesecondary/), or, on a type with [`TurretCount`](/keys/turretcount/) above zero, the numbered [`Weapon1`](/keys/weapon1/) to `WeaponN` keys. So `ClearAllWeapons=yes` overrides a weapon set in the same section.

Slots from the third on are not cleared. On a type with numbered weapons, [`Weapon3`](/keys/weapon3/) and later keep their weapons.

An object left with no weapon in its primary slot cannot fire.

```ini title="rules.ini"
[MYUNIT] ; a UnitType registered in [VehicleTypes]
Primary=MYGUN
ClearAllWeapons=yes ; MYUNIT ends up with no primary or secondary weapon
```
