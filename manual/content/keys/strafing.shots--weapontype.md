---
key: Strafing.Shots
scope: weapontype
summary: "How many shots an aircraft fires with this weapon in one strafing run."
see_also: [Burst, "system:aircraft-operations"]
when_omitted:
  kind: value
  value: "5"
---

`Strafing.Shots` is a Phobos key. An aircraft whose primary weapon makes it [strafe](/systems/aircraft-operations/#attack-passes) fires this many shots in one run, spaced by the weapon's [`ROF`](/keys/rof/#scope-weapontype). The first shot fires the weapon's whole [`Burst`](/keys/burst/); the others fire one shot each. The run still uses one point of [`Ammo`](/keys/ammo/), however many shots it holds. After the last shot the aircraft flies on for the time it takes to cover the weapon's range plus four cells, then starts its next run or returns to base. Values below `1` are read as `1`.

Only the primary weapon's value counts. A spy plane does not read the key.

```ini title="rulesmd.ini"
[MyBomb] ; example WeaponType
Strafing.Shots=3
```
