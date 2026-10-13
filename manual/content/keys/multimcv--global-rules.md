---
key: MultiMCV
scope: global-rules
label: No effect
summary: Accepted in rules files. The country a construction yard acts as does not limit what it builds, so the key changes nothing.
see_also: [BuildConst, BaseUnit, ActsLike, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

`MultiMCV` no longer changes what a construction yard builds, so `yes` and `no` behave the same. A yard builds each structure whose [`Owner`](/keys/owner/) list shares a country with the yard type's `Owner` list, whichever country the yard acts as. The key is still read, so a rules file that sets it loads without error.
