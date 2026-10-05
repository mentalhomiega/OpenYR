---
key: CanBeReversed
summary: Whether grinding up an object of this type in a reverse engineering structure teaches its owner anything.
see_also: [ReverseEngineersVictims, ReversedAs, "system:production"]
when_omitted:
  kind: value
  value: "yes"
---

The key is read on the victim's type. With `CanBeReversed=no`, a structure with [`ReverseEngineersVictims=yes`](/keys/reverseengineersvictims/) that grinds up an infantryman or vehicle of this type teaches its owner nothing, including the type that [`ReversedAs`](/keys/reversedas/) names. The grinding still pays its refund.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType that cannot be reverse engineered
CanBeReversed=no
```
