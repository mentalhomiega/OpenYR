---
key: Cloneable
summary: With no, a cloning structure does not copy a soldier of this type.
see_also: [Cloning, "system:production"]
when_omitted:
  kind: value
  value: "yes"
---

`Cloneable=no` keeps every [`Cloning=yes`](/keys/cloning/) structure from releasing a copy of an infantryman of this type. The barracks still releases the infantryman itself.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType
Cloneable=no
```
