---
key: PackupSound
summary: "The sound a structure makes as it is sold or packs up into its vehicle."
see_also: [BuildupSound, UndeploySound, SellSound]
when_omitted:
  kind: value
  value: none
---

A structure plays this sound at its position when its sale begins, which is also when it packs up into the vehicle named by [`UndeploysInto`](/keys/undeploysinto/). It plays only when the structure belongs to a player-controlled house and is not a one-cell structure that names an `UndeploysInto` vehicle. When the structure is sold outright, it plays in addition to [`SellSound`](/keys/sellsound/).

```ini title="rulesmd.ini"
[MYSILO] ; example BuildingType
PackupSound=MYSILO_Packup
```
