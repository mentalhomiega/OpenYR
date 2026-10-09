---
key: SellBack
summary: The base IQ a house must have before the computer will sell a building it cannot afford to repair.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "2"
---

A computer house sells a badly damaged building it cannot afford to repair only if its base [`IQ`](/keys/iq/) is at or above this value. The base IQ is the house's `IQ=` value from the map. A computer house that a skirmish or multiplayer match sets up has base IQ `0`, so it passes this test only when this value is `0`. A house the computer takes over keeps the base IQ it had.

The house's tech level does not decide whether it sells. It sets how soon the sale happens. On each frame that the other conditions hold, a random number from `0` through `50` must come out below the house's tech level. At tech level 5 that happens on about one frame in ten, and above tech level 50 it happens every time. Because the draw repeats, a low tech level delays the sale but does not prevent it. A house at tech level `0` never passes the draw.

The other conditions include money below [`CreditReserve`](/keys/creditreserve/), damage from a house that is not an ally, no trigger tag on the building, and health below [`ConditionRed`](/keys/conditionred/). [When the computer repairs](/systems/repair/#when-the-computer-repairs) lists all of them in order.
