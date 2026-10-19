---
key: DropPod.Minimum
scope: superweapontype
summary: "Sets the fewest pods a drop pod weapon lands."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] DropPodInfantryMinimum"
---

The number of pods is picked at random between this value and [`DropPod.Maximum=`](/keys/droppod.maximum/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
DropPod.Minimum=3
```

See [Drop pods](/systems/superweapons/#drop-pods).
