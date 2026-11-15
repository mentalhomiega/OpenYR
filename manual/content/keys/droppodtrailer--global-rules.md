---
key: DropPodTrailer
scope: global-rules
label: Smoke trail of a falling drop pod
summary: "The animation a falling drop pod leaves behind every six frames."
see_also: [DropPodWeapon, "system:drop-pods"]
when_omitted:
  kind: computed
  note: "SMOKEY, and only while DropPodWeapon is set."
---

`DropPodTrailer` is an Ares key in `[General]`. Once it is set, a falling drop pod leaves the named animation at its position every six frames, whether or not [`DropPodWeapon`](/keys/droppodweapon/) is set. `none` leaves no trail. Without the key, a pod leaves the `SMOKEY` animation, and only while `DropPodWeapon` is set; the stock rules do not register `SMOKEY` in `[Animations]`, so their pods leave no trail.

```ini title="rulesmd.ini"
[General]
DropPodTrailer=V3TRAIL ; an AnimType registered in [Animations]
```

See [Descent and airborne effects](/systems/drop-pods/#descent-and-airborne-effects).
