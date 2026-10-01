---
key: NukeMaker
summary: "Makes this warhead drop the NukePayload weapon on the target instead of exploding."
see_also: [Vertical, DetonationAltitude, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A projectile whose warhead is `NukeMaker=yes` does no damage where it explodes. Instead, the projectile of the `NukePayload` weapon appears above its target, at the exploding projectile's [`DetonationAltitude`](/keys/detonationaltitude/) over the ground, and falls straight down. The payload carries `NukePayload`'s damage and warhead.

```ini title="rulesmd.ini"
[NukeMaker] ; example WarheadType
NukeMaker=yes
```

The weapon name `NukePayload` is fixed. Without a `[NukePayload]` section with a projectile, nothing falls. [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile) covers the nuclear missile.
