---
key: LightningDamage
summary: "The damage each bolt of a lightning storm deals."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "200"
---

Each bolt deals this much damage through [`LightningWarhead`](/keys/lightningwarhead/) where it strikes, with no attacker. [Lightning storm](/systems/superweapons/#lightning-storm) covers the storm.

```ini title="rulesmd.ini"
[General]
LightningDamage=250
```

The ion storm uses [`IonLightningDamage`](/keys/ionlightningdamage/) instead.
