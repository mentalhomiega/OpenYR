---
key: ProjectileRange
summary: Read from rules files, but no shot's flight depends on it.
see_also: ["Range", "Ranged", "Projectile"]
when_omitted:
  kind: value
  value: "390.625"
---

No projectile uses this value. A rules file that sets it still loads, but no shot's flight, detonation or fuse depends on it. To set how far a weapon fires, use [`Range=`](/keys/range/). [`Ranged=yes`](/keys/ranged/) affects the fuse, not the distance flown.
