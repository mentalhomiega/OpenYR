---
key: Soylent
summary: A fixed number of credits that selling an object of this type refunds, in place of its price's refund.
see_also: [RefundPercent, Cost]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rules.ini"
[MYTANK]      ; example UnitType
Cost=1000
Soylent=200   ; selling it at a service depot returns 200 credits, times its owner's country multiplier
```

`Soylent=` sets the credits every house receives when it sells an object of this type. The value is multiplied by the owner's country multiplier for the object's category, such as [`CostUnitsMult=`](/keys/costunitsmult/) for a vehicle, and the product is truncated to whole credits. [`RefundPercent`](/keys/refundpercent/), the factory plant bonuses and the difficulty's [`Cost=`](/keys/cost/#scope-difficulty-settings) do not scale it.

The default `0` keeps the usual refund: the object's price, times `RefundPercent` for a human player's house. Any other value replaces that refund, a negative one included, which is then paid as a negative amount that takes credits from the house.

The value replaces the refund in every payment `RefundPercent` lists, which covers selling a structure, a vehicle or aircraft at a service depot, or an upgrade, and the compensation for a structure that cannot undeploy. It also replaces the payment for an object fed into a [`Grinding=yes`](/keys/grinding/) structure. An upgrade's sale uses the upgrade type's `Soylent=`, and undeploy compensation uses the structure's.
