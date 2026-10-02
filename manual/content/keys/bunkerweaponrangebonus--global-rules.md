---
key: BunkerWeaponRangeBonus
scope: global-rules
label: 'Bunkered vehicle range bonus'
see_also: [BunkerDamageMultiplier, BunkerROFMultiplier, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "2"
---

A vehicle in a bunker can fire at targets this many whole cells beyond its weapons' `Range`. Weapons with arcing projectiles do not gain the bonus, because their reach depends on the projectile's speed.
