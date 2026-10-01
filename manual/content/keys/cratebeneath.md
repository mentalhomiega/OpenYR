---
key: CrateBeneath
summary: "Leaves a crate where this structure stood when it is destroyed."
see_also: [CrateBeneathIsMoney, "system:crates"]
when_omitted:
  kind: value
  value: "no"
---

With `CrateBeneath=yes`, a destroyed structure of this type leaves a crate on or next to its center cell once it has left the map. [Crates left by destroyed structures](/systems/crates/#crates-left-by-destroyed-structures) gives where it lands and when none appears. The crate gives money with [`CrateBeneathIsMoney=yes`](/keys/cratebeneathismoney/) and a random result otherwise.

```ini title="rulesmd.ini"
[MYCIVBUILDING] ; example BuildingType
CrateBeneath=yes
CrateBeneathIsMoney=yes
```
