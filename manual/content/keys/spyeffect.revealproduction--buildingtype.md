---
key: SpyEffect.RevealProduction
scope: buildingtype
summary: "Lets the house that spied on a SpyEffect.Custom structure see its production, power and money."
see_also: ["SpyEffect.Custom"]
when_omitted:
  kind: value
  value: "no"
---

The key matters only for a structure with `SpyEffect.Custom=yes`. While the player's house has spied on such a structure and the player selects it, the structure shows the owner's figures only with `SpyEffect.RevealProduction=yes`. Without the key the selected structure shows nothing. A structure without `SpyEffect.Custom` keeps the stock display of every spied structure.

With the key, the selected structure shows:

- the owner's power output and drain, when its `Power=` is above 0;
- the cameo of what it is producing, when it is a factory;
- the owner's money below the power figures, when its `Storage=` is above 0.

The owner of the structure, and allies that share its view, see its power figures when they select it, whatever the key says. The label that Ares gives a `Fake=yes` structure is not shown.

```ini title="rulesmd.ini"
[GAPOWR] ; example BuildingType
SpyEffect.Custom=yes
SpyEffect.RevealProduction=yes
```
