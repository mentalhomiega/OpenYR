---
key: PreImpactAnim
summary: "An animation played where a projectile with this warhead hits, before the blast goes off."
see_also: [AnimList, "system:projectile-flight", "system:superweapons"]
when_omitted:
  kind: context-dependent
  note: "`NUKEBALL` for the warhead named `NUKE`, otherwise none, so the blast goes off at once."
---

When a projectile whose warhead sets `PreImpactAnim` goes off, the projectile stops where it is and plays this animation there. The blast, with its damage, explosion animation and the effects of the warhead, goes off when the animation ends. The projectile is not drawn in the meantime. A looping animation never ends, so the blast never goes off.

On the warhead named `NUKE` the key replaces `NUKEBALL`, and the map flash and the radar event of the nuclear blast still happen. On any other warhead, only the animation is added.

```ini title="rulesmd.ini"
[MyChargeWH] ; example Warhead
PreImpactAnim=MyChargeUp
```
