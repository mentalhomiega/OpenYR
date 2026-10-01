---
key: ForceShieldPlayFadeSoundTime
summary: "How many frames before the force shield ends its fading sound plays."
see_also: [SpecialSound, "system:superweapons"]
when_omitted:
  kind: value
  value: "50"
---

The force shield weapon's [`SpecialSound`](/keys/specialsound/) plays this many frames before [`ForceShieldDuration`](/keys/forceshieldduration/) runs out.

```ini title="rulesmd.ini"
[General]
ForceShieldPlayFadeSoundTime=75
```

The sound plays only while the house still holds the weapon.
