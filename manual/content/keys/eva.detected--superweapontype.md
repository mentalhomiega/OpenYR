---
key: EVA.Detected
scope: superweapontype
summary: "Names the announcer line played when an enemy builds the structure that grants the weapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The behavior's own line"
---

An announcer event from the EVA file. Outside a campaign, it plays when a house that is neither the player nor an ally of the player places the structure. `none` plays nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.Detected=EVA_NuclearSiloDetected
```

See [Announcements](/systems/superweapons/#announcements).
