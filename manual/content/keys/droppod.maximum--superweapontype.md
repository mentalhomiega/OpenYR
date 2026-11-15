---
key: DropPod.Maximum
scope: superweapontype
summary: "Sets the most pods a drop pod weapon lands."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] DropPodInfantryMaximum"
---

The number of pods is picked at random between [`DropPod.Minimum=`](/keys/droppod.minimum/) and this value. A value below the minimum is raised to it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
DropPod.Maximum=6
```

See [Drop pods](/systems/superweapons/#drop-pods).
