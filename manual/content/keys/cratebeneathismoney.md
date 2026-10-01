---
key: CrateBeneathIsMoney
summary: "Makes the crate a CrateBeneath structure leaves a money crate."
see_also: [CrateBeneath, "system:crates"]
when_omitted:
  kind: value
  value: "no"
---

With `CrateBeneathIsMoney=yes`, the crate a [`CrateBeneath`](/keys/cratebeneath/) structure leaves is a money crate. In a campaign, a money crate gives the result set by [`WoodCrate`](/keys/woodcrate/), as a crate drawn into the map does. Without [`CrateBeneath=yes`](/keys/cratebeneath/) this key has no effect.

```ini title="rulesmd.ini"
[MYCIVBUILDING] ; example BuildingType
CrateBeneath=yes
CrateBeneathIsMoney=yes
```
