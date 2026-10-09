---
key: SovietSurvivorDivisor
scope: global-rules
label: Soviet survivor divisor
summary: The credits of a crewed structure's refund that make one survivor, for a house on the second side listed under [Sides].
see_also: ["system:capture", AlliedSurvivorDivisor, ThirdSurvivorDivisor, Crewed, RefundPercent, Soylent]
when_omitted:
  kind: value
  value: "100"
---

`SovietSurvivorDivisor` sets how many credits of a sale refund make one survivor when a [`Crewed=yes`](/keys/crewed/) structure is destroyed or sold by a house on the second side in `[Sides]`. In the stock rules that side is Nod. Such a structure's survivor count is its refund divided by this value, rounded down and limited to between 1 and 5.

```ini title="rules.ini"
[General]
SovietSurvivorDivisor=250  ; a refund of 1,000 credits gives four survivors
```

The refund is the amount a sale of the structure returns, as [`RefundPercent`](/keys/refundpercent/) works it out, with the structure's [`Soylent=`](/keys/soylent/) value in its place when its type sets one. A refund smaller than the divisor still gives a count of one.

A captured structure uses twice this value, which roughly halves its survivors. `SovietSurvivorDivisor=0` gives no survivors to a structure whose owner is on the second side.

This key sets only the count. [Survivors](/systems/capture/#survivors) covers the odds for each footprint cell and the divisors for the other sides.
