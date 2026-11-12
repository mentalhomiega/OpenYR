---
key: AllowParallelAIQueues
summary: Whether a computer house builds in every one of its factories of a kind at once.
see_also: ["system:production", AllowBypassBuildLimit]
when_omitted:
  kind: value
  value: "yes"
---

`AllowParallelAIQueues` is an Ares key, read from `[GlobalControls]`. With `yes`, each factory a computer house owns picks and builds its own object, so a house with two war factories builds two vehicles at a time. With `no`, a factory waits while another factory of the same owner that builds the same kind of object, land or naval, is producing. The human player's production is not affected.

The Ares manual describes the key as stopping extra factories from building a copy of each unit. How Ares does this was not observed; the game reads the key as one factory of a kind producing at a time.

```ini title="rulesmd.ini"
[GlobalControls]
AllowParallelAIQueues=no
```
