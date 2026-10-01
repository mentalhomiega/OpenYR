---
key: InfantryAbsorb
summary: "Lets this structure take in its owner's infantry, each adding ExtraPower to its output."
see_also: [UnitAbsorb, ExtraPower, Passengers, "system:power"]
when_omitted:
  kind: value
  value: "no"
---

An `InfantryAbsorb=yes` structure takes in its owner's infantry. A player who points one at the structure gets the enter cursor while there is room. The soldier heads for the structure and goes inside when it reaches one of its cells. The structure holds as many as its type's [`Passengers`](/keys/passengers/), and each one inside adds [`ExtraPower`](/keys/extrapower/) to the structure's power output.

```ini title="rulesmd.ini"
[MYREACTOR] ; example BuildingType
InfantryAbsorb=yes
Passengers=5
ExtraPower=100
```

The owner lets them out with the Deploy command while the structure is selected, or by clicking the structure while it is the only object selected. They come out one at a time onto free cells next to the structure. Selling the structure lets everyone out at once; destroying it kills everyone inside. A soldier with no room to stand when it comes out is removed from the game.

Several of them can be sent at the same time. Any that find the structure full give up the order.
