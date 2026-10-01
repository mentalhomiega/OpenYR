---
key: IonCannonWarhead
summary: The WarheadType an ion cannon blast applies its damage through.
see_also: [IonCannonDamage, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

An ion cannon blast deals [`IonCannonDamage`](/keys/ioncannondamage/) through this warhead to objects within its [`CellSpread`](/keys/cellspread/) of the impact point. The warhead's [`PercentAtMax`](/keys/percentatmax/) sets how damage falls off with distance, its [`Verses`](/keys/verses/) table sets what each armor takes, and [`Bright=yes`](/keys/bright/#scope-warheadtype) adds a flash. `IonCannonDamage` covers kill credit, immune objects and cells under a bridge.

The [Ion-cannon strike...](/mapping/actions/taction-ion-cannon/) trigger action uses the same warhead. It sets off a blast directly, without a superweapon.

:::caution[This assignment also changes bridge damage]
An explosion with this warhead damages a bridge span without the [`BridgeStrength`](/keys/bridgestrength/) roll, provided the warhead sets [`Wall=yes`](/keys/wall/#scope-warheadtype) and bridge destruction is on. Any weapon that uses this same warhead also brings bridges down without the roll.
:::
