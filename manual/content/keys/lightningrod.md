---
key: LightningRod
summary: Raises the type's chance of being picked as the target of an ion storm lightning bolt, and draws lightning storm clouds to a structure.
see_also: [IonImmune, IonLightningRandomness, "LightningRod.Modifier", LightningRodRange, "Lightning.IgnoreLightningRod", "system:ion-storms", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[GAPOWR]
LightningRod=yes
```

Each aimed bolt draws its target from a [candidate list](/systems/ion-storms/#where-it-strikes). An object without the flag enters that list with a 2% chance. With the flag:

- a structure that is switched on enters with a 42% chance;
- a vehicle or infantryman whose locomotor has power enters with a 12% chance.

A switched-off structure, or a vehicle or infantryman whose locomotor has lost power, keeps the 2% chance. Aircraft never enter the list, with or without the flag.

In Ares rules, which declare armor types in `[ArmorTypes]`, a structure with the flag also draws the clouds of a [lightning storm](/systems/superweapons/#lightning-storm); Yuri's Revenge reads the flag but its storm ignores it. A cloud placed at random forms over the nearest such structure within [`LightningRodRange`](/keys/lightningrodrange/) cells, and [`LightningRod.Modifier=`](/keys/lightningrod.modifier/) scales the damage its bolts do to it. A weapon with [`Lightning.IgnoreLightningRod=yes`](/keys/lightning.ignorelightningrod/) skips both.

:::caution[Lightning rods on ion-immune teams]
A member of a team whose TeamType sets [`IonImmune=yes`](/keys/ionimmune/) is normally left out of the candidate list. With `LightningRod=yes` it is put back in and bolts can be aimed at it, but their blast still does it no damage.
:::
