---
key: ForceShieldDuration
summary: "How many frames the force shield protects its structures."
see_also: [ForceShieldRadius, ForceShieldPlayFadeSoundTime, "system:superweapons"]
when_omitted:
  kind: value
  value: "400"
---

A structure under the force shield takes no damage for this many frames, as under the Iron Curtain. [Force shield](/systems/superweapons/#force-shield) covers the weapon.

```ini title="rulesmd.ini"
[General]
ForceShieldDuration=500
```

A second shield or Iron Curtain restarts the protection at its own length.
