---
key: Technician
summary: The InfantryType a crewed vehicle leaves when its house has no side, and sometimes when a crewed object is armed.
see_also: ["system:capture", AlliedCrew, Crewed]
when_omitted:
  kind: value
  value: none
---

A [`Crewed=yes`](/keys/crewed/) structure or vehicle produces this type as a survivor in two cases:

- its owning house names no [`Side=`](/keys/side/#scope-housetype), in which case every survivor is of this type. A structure whose house names no side releases no survivors, so this case applies only to a vehicle;
- its owning house names a side and the object has a primary weapon, in which case each survivor is of this type on a 15% roll.

Before either test, a structure that builds structures and has never been captured takes a one-in-four roll for the `[General]` [`Engineer`](/keys/engineer/#scope-global-rules) type. Other survivors take their side's crew type, such as [`AlliedCrew`](/keys/alliedcrew/). The `AlliedCrew` page gives the full order.

Each survivor is chosen separately, so one object can leave a mix of types.

Every country in the stock rules names a side, so the first case arises only for a country that a mod leaves without one.

A survivor whose type is [`Nominal=yes`](/keys/nominal/) can be marked as a technician, whether its type came from this key, a side's crew key or `Engineer`:

- A sold structure marks every such survivor.
- A destroyed structure marks such survivors only if it has build-up artwork.
- A vehicle's escaping crew is never marked.

A marked survivor is named with the game's "Technician" text, cannot be picked up as a civilian evacuee, and is not counted among its house's infantry.

:::caution[With no type named, a survivor of that type is left out]
With no type named, a survivor that would be of this type is left out of a sold or destroyed structure, and the rest still come out. That happens on 15% of the survivors of an armed `Crewed=yes` structure whose house is on one of the first three sides. A structure whose every survivor has no type releases none. The stock rules name `CTECH`.
:::
