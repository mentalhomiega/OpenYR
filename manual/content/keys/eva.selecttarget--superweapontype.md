---
key: EVA.SelectTarget
scope: superweapontype
summary: "Names the announcer line played when the cameo starts aiming."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The select-target line"
---

An announcer event from the EVA file. `none` plays nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.SelectTarget=none
```

See [Announcements](/systems/superweapons/#announcements).
