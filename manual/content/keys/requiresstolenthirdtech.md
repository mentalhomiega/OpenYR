---
key: RequiresStolenThirdTech
summary: Makes the type buildable only after its house has stolen the third side's technology.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "no"
---

With `RequiresStolenThirdTech=yes`, a house may build the type only once it has stolen the third side's technology. A house steals it when one of its spies walks into another house's [`BuildTech`](/keys/buildtech/) structure whose [`AIBasePlanningSide`](/keys/aibaseplanningside/) points at the third side, as [Infiltrating it](/systems/capture/#infiltrating-it) describes. Until then the type cannot be built.

```ini title="rulesmd.ini"
[MyCommando] ; example InfantryType
RequiresStolenThirdTech=yes
```
