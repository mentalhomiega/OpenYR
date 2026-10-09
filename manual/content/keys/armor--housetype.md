---
key: Armor
scope: housetype
label: Armor bias
see_also: ["system:difficulty", Verses]
when_omitted:
  kind: value
  value: "1.0"
---

Damage to the objects of a house of this country is divided by this value. A value above 1 makes them harder to destroy, and one below 1 makes them easier.

```ini title="rules.ini"
[NOD]
Armor=1.25 ; example: NOD's objects take 20% less damage from each hit
```

The value applies in campaign and skirmish games alike. A difficulty section's [`Armor=`](/keys/armor/#scope-difficulty-settings) does not multiply it, as [Difficulty settings](/systems/difficulty/#how-the-figures-are-combined) explains.

The division applies to every hit except damage that bypasses armor, such as a planted C4 charge. Healing is never reduced. The division comes before the veteran armor bonus, and the two together cannot bring a hit below 1 point. The warhead's [`Verses`](/keys/verses/) percentage and the falloff with distance are applied after that.

:::caution[Keep the value above 0]
A `0` here reduces every hit on that house's objects to 1 point before `Verses` and the distance falloff are applied, however strong the weapon.
:::
