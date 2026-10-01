---
key: IvanBomb
summary: "Makes a warhead fix a time bomb to its target instead of hurting it."
see_also: [IvanTimedDelay, IvanDamage, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "no"
---

A soldier's weapon with this warhead fixes an [Ivan bomb](/systems/ivan-bombs/) to what it hits. Weapons fired by anything other than infantry plant nothing.

```ini title="rulesmd.ini"
[MyIvanBomb] ; example Warhead
IvanBomb=yes
```
