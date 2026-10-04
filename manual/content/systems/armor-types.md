---
title: Armor types
summary: "How a mod declares armor types beyond the eleven stock classes, sets warhead damage against any armor, and switches off forced fire, retaliation and automatic targeting per armor."
category: combat-targeting
keys:
  - Armor
  - Verses
related:
  - type: enum
    id: ArmorType
  - type: system
    id: warheads
  - type: system
    id: target-selection
---

An `[ArmorTypes]` section adds armor types to the eleven [stock armor classes](/reference/enums/armor/). A warhead then sets its damage against any armor type, stock or declared, with `Versus.<armor>` entries, and can allow or forbid forced fire, retaliation and automatic targeting against it. The key names follow the Ares documentation's [additional armor types and Verses](https://ares-developers.github.io/Ares-docs/new/additionalarmortypesandverses.html) page; this page describes only what this engine does with them.

## Declare an armor type

List each new type in `[ArmorTypes]` in the rules or in a map's rule overrides. The key is the armor name. The value sets what every warhead does against it until that warhead says otherwise:

| Value | Every warhead starts at |
| --- | --- |
| An armor declared earlier, one of the eleven stock classes or an earlier entry | Its own multiplier against that armor |
| A percentage such as `75%`, or a fraction such as `0.75` | That figure |
| Anything else, including an empty value or a name declared later in the list | `100%` |

A value with a `%` sign is read as a whole number, so `12.5%` counts as `12%`. A value that starts with a digit or a point is read as a fraction. A value that names no earlier armor type also writes a line to the debug log.

An armor type that is already known, in any letter case, is skipped. Declare a type in the same file as, or an earlier file than, the first `Armor=` that names it. A name the engine does not know when it reads `Armor=` counts as `none`.

An object uses a declared type through its [`Armor=`](/keys/armor/), exactly as it uses a stock class.

```ini title="rules.ini"
[ArmorTypes]
Shielded=heavy ; example: warheads start at their heavy figure against it
Ghostly=25%    ; example: warheads start at 25% against it

[SHIELDEDTANK]
Armor=Shielded
```

A declaration is cleared each time the rules are loaded for a scenario. The rules files declare first, and the map's `[ArmorTypes]` adds to them for that scenario.

## Set damage with Versus

`Versus.<armor>` in a warhead's section sets that warhead's multiplier against the named armor type. It works for the eleven stock classes too, and for those it replaces the matching entry of the warhead's [`Verses`](/keys/verses/) list. The value is a percentage or a fraction, read as `Verses` entries are.

```ini title="rules.ini"
[MyShellWH]
Verses=100%,100%,100%,100%,100%,100%,100%,100%,100%,100%,100%
Versus.heavy=50%    ; replaces the Verses entry for heavy
Versus.Shielded=10% ; its own figure against the declared type
```

A declared armor type with no `Versus.<armor>` in a warhead follows its `[ArmorTypes]` default each time the warhead is used. A type declared with a base therefore follows the warhead's current multiplier against that base, including a later change to it.

Reading a warhead's section again, for example from a map's rule overrides, restarts its `Verses` list at `100%` when the section has no `Verses=`, and that includes any stock figure an earlier `Versus.<armor>` set. A `Versus.<armor>` entry for a declared type that an earlier file set is kept until another file sets it again.

The multiplier takes the place of the `Verses` entry everywhere damage and targeting read it, such as the [armor step of damage](/systems/warheads/#what-the-target-loses) and the [threat score](/systems/target-selection/#the-threat-score).

## Switch forced fire, retaliation and automatic targeting

Three boolean entries control one warhead against one armor type. They apply to the stock classes and the declared types alike.

| Key | When `no` |
| --- | --- |
| `Versus.<armor>.ForceFire` | The weapon never fires at an object of that armor, whether ordered or not |
| `Versus.<armor>.Retaliate` | An object armed with this weapon does not fire back at an attacker of that armor |
| `Versus.<armor>.PassiveAcquire` | An object armed with this weapon does not choose an object of that armor as a target on its own |

An entry that is present and starts with anything other than `n`, `f` or `0` counts as `yes`. An entry that is absent follows the multiplier:

| Switch | Allowed unless the multiplier is |
| --- | --- |
| `ForceFire` | exactly `0` |
| `PassiveAcquire` | exactly `0` |
| `Retaliate` | below `1%` |

The `Retaliate` threshold matches Yuri's Revenge: a `1%` multiplier still retaliates and anything smaller does not. A `yes` entry allows the action whatever the multiplier is, so `Versus.Shielded.ForceFire=yes` lets a weapon with `0%` against `Shielded` fire at it, and then it deals no damage.

Switches are not inherited. A declared armor type reads its own three entries and does not take them from its base, so `Versus.Shielded.Retaliate=no` has to be written even when `Shielded` is declared with a base.

### Forced fire at a `0%` armor

A weapon whose warhead has `0%` against a target's armor does not fire at that target even when ordered to, as in Yuri's Revenge. This holds for the stock classes too. Write `Versus.<armor>.ForceFire=yes` to allow the shot. The rule applies only to targets that are vehicles, infantry, aircraft or structures.

The retaliation switch is the warhead's own entry in the [retaliation table](/systems/target-selection/#retaliation), in place of that table's `0%` test. The passive switch is tested while an object evaluates a candidate, only for an object that has a weapon, using the weapon it would select against that candidate.
