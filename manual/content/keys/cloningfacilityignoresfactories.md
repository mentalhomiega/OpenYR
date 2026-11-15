---
key: CloningFacilityIgnoresFactories
summary: "Whether a cloning structure that also builds objects is passed over when vehicles are copied."
see_also: [CloningFacility, Cloneable]
when_omitted:
  kind: value
  value: "yes"
---

`CloningFacilityIgnoresFactories` is our own key, read from `[General]`. It decides what happens to a structure that sets [`CloningFacility=yes`](/keys/cloningfacility/) and also builds objects of its own. With `yes`, such a structure makes no copies, so only structures that are not factories clone vehicles. With `no`, it copies as any other cloning structure does.

The key keeps a mod's war factory from copying its own tanks by accident when the cloning flag is set on it. A structure counts as a factory if it builds anything, whatever the type.

```ini title="rulesmd.ini"
[General]
CloningFacilityIgnoresFactories=no
```
