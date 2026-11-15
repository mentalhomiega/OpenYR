---
key: EVA.Impatient
scope: superweapontype
summary: "Names the announcer line played when the cameo is clicked before the weapon is ready."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "ImpatientVoice"
---

An announcer event from the EVA file. `none` plays nothing. A stopped countdown keeps [`SuspendVoice=`](/keys/suspendvoice/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.Impatient=EVA_BattlePowerReady
```

See [Announcements](/systems/superweapons/#announcements).
