---
key: ForceShieldRadius
summary: "How far from its target the force shield reaches, in cells."
see_also: [ForceShieldDuration, "system:superweapons"]
when_omitted:
  kind: value
  value: "10"
---

The force shield protects each structure of the firing house or its allies whose center is less than this many cells from the target, measured in a straight line. [Force shield](/systems/superweapons/#force-shield) covers the weapon.

```ini title="rulesmd.ini"
[General]
ForceShieldRadius=4
```

Vehicles and infantry are never shielded.
