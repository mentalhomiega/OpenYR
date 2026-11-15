---
key: NoAmmoWeapon
scope: aircrafttype
summary: "The only weapon an object with limited ammunition fires once its ammunition is low."
see_also: [NoAmmoAmount, Ammo, DeployFireWeapon]
when_omitted:
  kind: value
  value: "-1, so the weapons are used as usual"
---

`NoAmmoWeapon=0` or `1` makes an object that has run low on ammunition fire only its primary or its secondary weapon, whatever the target. The other weapon refuses to fire at that time. The weapon is used once the object's ammunition is at or below [`NoAmmoAmount`](/keys/noammoamount/). A type with unlimited ammunition, which sets no [`Ammo=`](/keys/ammo/), never switches. A deploy-fire weapon of a `DeployFire=yes` soldier or vehicle keeps its own choice.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Ammo=3
Primary=BigGun
Secondary=MachineGun
NoAmmoWeapon=1  ; with no shells left, only the machine gun fires
```
