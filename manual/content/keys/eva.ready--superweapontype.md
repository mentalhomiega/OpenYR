---
key: EVA.Ready
scope: superweapontype
summary: "Names the announcer line played when the weapon is charged."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The behavior's own line"
---

An announcer event from the EVA file, played to the owner. `none` plays nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.Ready=EVA_BattlePowerReady
```

See [Announcements](/systems/superweapons/#announcements).
