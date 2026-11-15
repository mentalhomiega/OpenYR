---
key: SW.AlwaysGranted
scope: superweapontype
summary: "Gives every house the weapon without a structure."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, every house that is not defeated holds the weapon from the start, charges it, and keeps it without a granting structure. The Ares availability rules still apply: [`SW.AllowPlayer=`](/keys/sw.allowplayer/), [`SW.AllowAI=`](/keys/sw.allowai/), [`SW.RequiredHouses=`](/keys/sw.requiredhouses/), [`SW.ForbiddenHouses=`](/keys/sw.forbiddenhouses/), [`SW.AuxBuildings=`](/keys/sw.auxbuildings/), [`SW.NegBuildings=`](/keys/sw.negbuildings/) and [`SW.Shots=`](/keys/sw.shots/) can withhold or withdraw it. A weapon with [`IsPowered=yes`](/keys/ispowered/) is suspended while the house is short of power, as any held weapon is.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AlwaysGranted=yes
```

See [Always granted](/systems/superweapons/#always-granted).
