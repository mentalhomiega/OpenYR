---
key: ZAdjust
summary: The depth bias an animation of this type starts with when its creator gives none.
see_also: [YSortAdjust, YDrawOffset, ActiveAnimZAdjust]
when_omitted:
  kind: value
  value: "0"
---

`ZAdjust` shifts the depth at which the animation is drawn. A negative value brings it toward the viewer, so it covers more of the objects around it, and a positive value pushes it back. The artwork does not move on screen.

An animation takes the value when it is created, unless the object that creates it passes a non-zero depth bias of its own, which replaces the type's value. An animation that a structure runs in one of its [animation slots](/systems/building-animations/#placement-and-draw-order) uses the slot's value instead, which is `0` when the slot sets none. To change the order of the animation among ground objects, use [`YSortAdjust`](/keys/ysortadjust/).

```ini title="artmd.ini"
[MYFLAME] ; example AnimType
ZAdjust=-20 ; drawn in front of objects it would otherwise sit behind
```
