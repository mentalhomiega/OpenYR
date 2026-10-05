---
title: Attached effects
summary: "Timed effects that a warhead puts on the objects it hits, or that an object type gives itself, changing speed, armor, firepower, rate of fire and cloaking and showing an animation."
category: combat-targeting
keys:
  - AttachEffect.Animation
  - AttachEffect.Duration
  - AttachEffect.TemporalHidesAnim
  - AttachEffect.SpeedMultiplier
  - AttachEffect.ArmorMultiplier
  - AttachEffect.FirepowerMultiplier
  - AttachEffect.ROFMultiplier
  - AttachEffect.Cloakable
  - AttachEffect.ForceDecloak
  - AttachEffect.DiscardOnEntry
  - AttachEffect.PenetratesIronCurtain
  - AttachEffect.Delay
  - AttachEffect.InitialDelay
  - AttachEffect.Cumulative
  - AttachEffect.AnimResetOnReapply
related:
  - type: system
    id: warheads
  - type: system
    id: cloaking
  - type: system
    id: temporal-weapons
  - type: system
    id: transports
---

An attached effect is a set of multipliers, an optional animation and cloaking rules that stay on one soldier, vehicle, aircraft or structure for a number of frames. A warhead attaches its effect to the objects it hits, and an object type can give its own effect to every object of the type. The keys follow the Ares documentation's [AttachEffect](https://ares-developers.github.io/Ares-docs/new/attacheffect.html) page; this page describes what this engine does with them.

```ini title="rulesmd.ini"
[Warheads]
900=SlowGoo ; example: a new warhead must be listed

[SlowGoo] ; example Warhead
AttachEffect.Duration=300           ; 300 frames
AttachEffect.SpeedMultiplier=0.5    ; half speed
AttachEffect.ROFMultiplier=2.0      ; reloads take twice as long
AttachEffect.Animation=TWLT100      ; shown on the object while the effect lasts
```

A weapon with `Warhead=SlowGoo` and a `Damage` other than `0` puts this effect on each object it hits.

An effect needs an [`AttachEffect.Duration`](/keys/attacheffect.duration/) other than `0`. With the default `0`, none of the other `AttachEffect` keys of that section does anything.

## Effects from a warhead

A warhead attaches its effect to every object its detonation damages, including the objects in its `CellSpread`. The effect is attached before the hit's damage is applied, so the hit that attaches it is not changed by its own armor multiplier. No effect is attached to an object when any of these holds:

- the warhead's `Verses` value against the object's armor is `0%`;
- the warhead does nothing to the object because of `AffectsAllies=no`, radiation, psionic or poison immunity, or a tank bunker;
- the object is under the Iron Curtain or a Force Shield, unless the warhead sets [`AttachEffect.PenetratesIronCurtain=yes`](/keys/attacheffect.penetratesironcurtain/#scope-warheadtype).

A weapon whose `Damage` is `0` does not detonate its warhead on anything, so it attaches nothing. A weapon with negative `Damage` attaches its effect as it heals.

When the object already carries an effect from the same warhead, the hit restarts that effect's duration and adds no second copy. With [`AttachEffect.Cumulative=yes`](/keys/attacheffect.cumulative/) every hit adds a new copy with its own duration instead. Effects from different warheads are always separate.

## An object type's own effect

When an object type's section sets `AttachEffect.Duration`, every object of that type attaches the effect to itself [`AttachEffect.InitialDelay`](/keys/attacheffect.initialdelay/) frames after it enters the game. When that effect ends, the object attaches it again after [`AttachEffect.Delay`](/keys/attacheffect.delay/) frames; a negative `Delay` means it is not attached again. While the object is under the Iron Curtain or a Force Shield, its own effect waits to be attached unless the type sets `AttachEffect.PenetratesIronCurtain=yes`.

```ini title="rulesmd.ini"
[FV] ; example VehicleType: half speed for 60 frames out of every 90
AttachEffect.Duration=60
AttachEffect.InitialDelay=10
AttachEffect.Delay=30
AttachEffect.SpeedMultiplier=0.5
```

## Duration and removal

An effect counts its duration down one frame at a time while the object is on the map. The count pauses while the object is inside a transport or structure and while a temporal weapon warps it out, and continues when it is back. A negative duration never runs out.

An effect is removed when:

- its duration runs out;
- the object is destroyed;
- the object leaves the map, for example by entering a transport or a structure, and the effect sets [`AttachEffect.DiscardOnEntry=yes`](/keys/attacheffect.discardonentry/#scope-warheadtype).

## Multipliers

While effects are attached, the object's speed, armor, firepower and rate of fire are multiplied by the product of the corresponding multipliers of all its effects. Two copies of an effect with `AttachEffect.FirepowerMultiplier=1.5` give a firepower of `2.25` times the usual. Each multiplier is applied together with the owner's country bonus and any crate or veteran bonus for the same value.

| Key | Changes | A value above `1` |
| --- | --- | --- |
| [`AttachEffect.SpeedMultiplier`](/keys/attacheffect.speedmultiplier/) | Movement speed | Moves faster |
| [`AttachEffect.ArmorMultiplier`](/keys/attacheffect.armormultiplier/) | Damage taken, divided by the value | Takes less damage |
| [`AttachEffect.FirepowerMultiplier`](/keys/attacheffect.firepowermultiplier/) | Damage of each shot fired | Deals more damage |
| [`AttachEffect.ROFMultiplier`](/keys/attacheffect.rofmultiplier/) | Reload time after each burst | Fires more slowly |

The reload time is set when the reload starts. A reload that started while an effect lasted keeps its length after the effect ends.

## Animation and cloaking

An effect with an [`AttachEffect.Animation`](/keys/attacheffect.animation/) shows that animation on the object, repeating until the effect ends. Each copy of a cumulative effect shows its own animation. The animation is removed while the object is off the map and from the moment the object starts to cloak until it is fully visible again, and it is shown again afterwards. The effect keeps working while its animation is hidden.

An effect with [`AttachEffect.Cloakable=yes`](/keys/attacheffect.cloakable/) lets the object cloak while the effect lasts, as a `Cloakable=yes` type does. An effect with [`AttachEffect.ForceDecloak=yes`](/keys/attacheffect.forcedecloak/) uncloaks the object each time it is attached or restarted.
