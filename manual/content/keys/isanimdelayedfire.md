---
key: IsAnimDelayedFire
summary: "Makes a structure charge its SpecialAnim for DelayedFireDelay frames before its weapon fires."
see_also: ["DelayedFireDelay", "system:prism-towers"]
when_omitted:
  kind: value
  value: "no"
---

`IsAnimDelayedFire=yes` makes a structure hold each shot back. When it has a target it can fire at, it plays its `SpecialAnim` in place of its `ActiveAnim` for [`DelayedFireDelay`](/keys/delayedfiredelay/) frames, then fires, as a Tesla coil does. When the `SpecialAnim` has played through, the `ActiveAnim` comes back. The key is read from the structure's art section.

Such a structure's `SpecialAnim` is not started when power returns, as it is for other animations that have `SpecialAnimPoweredLight=yes`. A charge in progress is dropped when the structure loses power.

```ini title="artmd.ini"
[NATSLA] ; example art section
IsAnimDelayedFire=yes
DelayedFireDelay=28
```
