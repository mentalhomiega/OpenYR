---
key: AlliedSolidTransparency
summary: "Lets SubjectToBuildings shots fly through solid buildings owned by the firer's allies."
see_also: [SubjectToBuildings, SolidHeight, AlliedWallTransparency]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a [`SubjectToBuildings=yes`](/keys/subjecttobuildings/) projectile is not stopped by a solid building owned by its firer's house or an ally. The building of the firer itself never stops it either way.

```ini title="rulesmd.ini"
[CombatDamage]
AlliedSolidTransparency=yes
```
