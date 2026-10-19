---
key: EVA.Activated
scope: superweapontype
summary: "Names the announcer line played when the weapon fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The behavior's own line"
---

An announcer event from the EVA file, played when the effect starts. `none` plays nothing. A behavior with no line of its own, such as `GenericWarhead`, stays silent without this key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.Activated=EVA_IronCurtainActivated
```

See [Announcements](/systems/superweapons/#announcements).
