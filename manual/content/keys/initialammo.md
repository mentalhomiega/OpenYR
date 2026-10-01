---
key: InitialAmmo
summary: "The shots a new object of this type starts with."
see_also: [Ammo]
when_omitted:
  kind: value
  value: "-1"
---

A new object of this type starts with this many shots in its [`Ammo`](/keys/ammo/) pool instead of a full one. The default `-1` starts it full. The pool then refills as `Ammo` describes, up to the `Ammo` value.

```ini title="rulesmd.ini"
[MYBOMBER] ; example AircraftType
Ammo=4
InitialAmmo=0   ; must rearm before its first sortie
```
