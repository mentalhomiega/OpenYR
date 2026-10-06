---
key: Verses
summary: The percentage of a weapon's damage the warhead delivers against each armor class, in armor order.
see_also: ["system:ai-base-building", "system:target-selection"]
when_omitted:
  kind: value
  value: "100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%"
  note: The default is applied whenever the warhead's section is present without the key, so a later file that contains the section and omits the key restores full damage against every armor class.
---

`Verses` lists eleven percentages, one for each [armor class](/reference/enums/armor/) in the order `none`, `flak`, `plate`, `light`, `medium`, `heavy`, `wood`, `steel`, `concrete`, `special_1`, `special_2`. Damage from this warhead is multiplied by the entry for the target type's [`Armor`](/keys/armor/#scope-aircrafttype) class. A percentage is read as a whole number, so `12.5%` counts as `12%`. A plain fraction such as `0.5` is accepted in place of `50%`, and entries after the eleventh are ignored. A list with fewer than eleven entries leaves the classes it does not reach as they were: `100%`, unless an earlier rules file set them.

```ini title="rules.ini"
[MyShellWH] ; example WarheadType
Verses=100%,90%,80%,70%,60%,50%,40%,30%,20%,10%,100% ; none, flak, plate, light, medium, heavy, wood, steel, concrete, special_1, special_2
```

A `0%` entry stops the warhead harming that class, and stops a weapon with that warhead firing at an object of that class even when ordered to. The multiplied damage is rounded down, so a small hit against a low percentage can come to nothing. [What the target loses](/systems/warheads/#what-the-target-loses) gives the full order.

An empty `Verses=` counts as leaving the key out.

A `Versus.<armor>` entry in the same section overrides the list's entry for that class and also reaches armor types a mod declares. [Armor types](/systems/armor-types/) describes both, along with the per-armor switches that can stop forced fire, retaliation or automatic targeting independently of the percentage.

The same table also decides how objects armed with this warhead choose and answer targets:

- An object with two weapons passes over one whose warhead has `0%` against the target's class, as [Which weapon the score assumes](/systems/target-selection/#which-weapon-the-score-assumes) describes.
- The [threat score](/systems/target-selection/#the-threat-score) weighs how well each side's warhead does against the other's armor.
- An object whose chosen weapon has less than `1%` against its attacker's class does not [retaliate](/systems/target-selection/#retaliation).
- When the computer's base is attacked, it does not [call up](/systems/base-attacked/#which-objects-qualify) an object whose primary weapon has `0%` against the attacker's class.
- `medium` and `wood` entries both of exactly `0%` confine infantry whose primary weapon uses this warhead to [infantry targets](/systems/target-selection/#what-each-kind-of-object-considers).
