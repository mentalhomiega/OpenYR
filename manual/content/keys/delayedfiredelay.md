---
key: DelayedFireDelay
summary: "Frames a prism tower charges before it fires or beams support."
see_also: ["system:prism-towers"]
when_omitted:
  kind: value
  value: "0"
---

A tower of the [`PrismType`](/keys/prismtype/) type [charges](/systems/prism-towers/) for this many frames before it fires its shot or sends its support beam. The key is read from the structure's art section.

```ini title="artmd.ini"
[ATESLA] ; example art section
DelayedFireDelay=56
```
