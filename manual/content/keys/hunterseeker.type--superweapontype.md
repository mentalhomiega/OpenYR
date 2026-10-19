---
key: HunterSeeker.Type
scope: superweapontype
summary: "Names the vehicle a hunter seeker launch sends out."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The side's HunterSeeker"
---

A vehicle type. Without it, the firing house's side supplies the seeker.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
HunterSeeker.Type=HUNTER
```

See [Hunter seeker](/systems/superweapons/#hunter-seeker).
