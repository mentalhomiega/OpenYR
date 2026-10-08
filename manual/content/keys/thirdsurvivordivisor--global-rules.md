---
key: ThirdSurvivorDivisor
scope: global-rules
label: Third survivor divisor
summary: The credits of a crewed structure's refund that make one survivor, for a house on the third side listed under [Sides].
see_also: ["system:capture", AlliedSurvivorDivisor, SovietSurvivorDivisor, Crewed, RefundPercent, Soylent]
when_omitted:
  kind: value
  value: "300"
---

`ThirdSurvivorDivisor` sets how many credits of a sale refund make one survivor when a [`Crewed=yes`](/keys/crewed/) structure is destroyed or sold by a house on the third side in `[Sides]`. The stock rules do not set it. Such a structure's survivor count is its refund divided by this value, rounded down and limited to between 1 and 5.

```ini title="rules.ini"
[General]
ThirdSurvivorDivisor=300  ; a refund of 1,500 credits gives five survivors
```

The refund is the structure's price, with its owner's [price multipliers](/keys/cost/#what-a-house-pays) applied. For a human house, the price is then multiplied by [`RefundPercent`](/keys/refundpercent/). A type with a [`Soylent=`](/keys/soylent/) value refunds that value instead. A refund smaller than the divisor still gives a count of one, including a `Soylent=0` refund of nothing.

A captured structure uses twice this value, which roughly halves its survivors. `ThirdSurvivorDivisor=0` gives no survivors to a structure whose owner is on the third side.

This key sets only the count. [Survivors](/systems/capture/#survivors) covers the odds for each footprint cell and the divisors for the other sides.
