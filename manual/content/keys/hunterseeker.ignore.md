---
key: HunterSeeker.Ignore
summary: Keeps a hunter-seeker drone from choosing objects of this type as its target.
see_also: [HunterSeeker]
when_omitted:
  kind: value
  value: "no"
---

A hunter-seeker drone picks its target at random from the enemy objects on the map. With `HunterSeeker.Ignore=yes` an object of this type is never in that list, even when nothing else is left, so a drone launched against a side that has only such objects finds no target. The setting protects objects that a drone should not spend itself on, such as cheap decoys or the objects of a mission.

```ini title="rulesmd.ini"
[MYTYPE] ; example type
HunterSeeker.Ignore=yes
```
