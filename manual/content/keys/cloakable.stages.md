---
key: Cloakable.Stages
summary: How many steps this object type takes to hide and to reappear.
see_also: [CloakingStages, CloakingSpeed, "system:cloaking"]
when_omitted:
  kind: inherited
  note: "[General] CloakingStages."
---

`Cloakable.Stages` replaces [`CloakingStages`](/keys/cloakingstages/) for objects of this type. A value of `0` or less, or no key, keeps the global count. A smaller count makes the object fade in fewer steps, so with the same [`CloakingSpeed`](/keys/cloakingspeed/) it hides and reappears sooner. [The four states](/systems/cloaking/#the-four-states) shows how the steps map to the object's look.

```ini title="rulesmd.ini"
[SUB] ; example VehicleType
Cloakable=yes
Cloakable.Stages=1 ; hides in one step
```
