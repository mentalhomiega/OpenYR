---
key: AttachEffect.Duration
scope: warheadtype
label: Effect duration
when_omitted:
  kind: value
  value: "0"
  note: "No effect is attached, and the other AttachEffect keys of the section do nothing."
---

`AttachEffect.Duration` is how many frames the effect this warhead attaches lasts. A negative value lasts until the object is destroyed or, with [`AttachEffect.DiscardOnEntry=yes`](/keys/attacheffect.discardonentry/#scope-warheadtype), leaves the map. A hit on an object that already carries this warhead's effect sets the remaining time back to this value, unless the warhead is [cumulative](/keys/attacheffect.cumulative/). The count pauses while the object is off the map or warped out by a temporal weapon; [Attached effects](/systems/attach-effects/#duration-and-removal) gives the full rules.

```ini title="rulesmd.ini"
[SlowGoo] ; example Warhead
AttachEffect.Duration=300 ; each hit attaches or restarts a 300-frame effect
AttachEffect.SpeedMultiplier=0.5
```
