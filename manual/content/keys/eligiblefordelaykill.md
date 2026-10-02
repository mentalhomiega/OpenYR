---
key: EligibleForDelayKill
summary: "Lets a CausesDelayKill warhead destroy the structure after a delay."
see_also: [CausesDelayKill]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a [`CausesDelayKill=yes`](/keys/causesdelaykill/) warhead drops the structure to 1 strength and destroys it after a delay instead of damaging it.

```ini title="rulesmd.ini"
[CAMISC01] ; Barrels
EligibleForDelayKill=yes
```
