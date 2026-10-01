---
key: Parasite
summary: "Makes a warhead put its firer inside the target instead of hurting it."
see_also: [LimboLaunch, Parasiteable, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

A weapon with this warhead puts its firer inside what it hits, from where the firer eats it with the same weapon. It works only as the primary weapon a vehicle, soldier or aircraft has when it is placed on the map. [Parasites](/systems/parasites/) covers getting in, the damage and coming out.

```ini title="rulesmd.ini"
[MyParasite] ; example Warhead
Parasite=yes
```
