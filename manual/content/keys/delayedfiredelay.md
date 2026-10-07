---
key: DelayedFireDelay
summary: "Frames a prism tower or other IsAnimDelayedFire structure charges before it fires or beams support."
see_also: ["IsAnimDelayedFire", "system:prism-towers"]
when_omitted:
  kind: value
  value: "0"
---

A tower of the [`PrismType`](/keys/prismtype/) type [charges](/systems/prism-towers/) for this many frames before it fires its shot or sends its support beam. So does any other structure with [`IsAnimDelayedFire=yes`](/keys/isanimdelayedfire/), such as a Tesla coil, before it fires. The key is read from the structure's art section.

```ini title="artmd.ini"
[ATESLA] ; example art section
DelayedFireDelay=56
```
