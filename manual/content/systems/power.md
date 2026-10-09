---
title: Power output and drain
summary: "How a house's power output and drain are totaled, and what a house short of power loses."
category: buildings-economy
keys:
  - Overpowerable
  - ElectricAssault
  - BuildSpeed
  - BuildTime
  - C4Warhead
  - CloakGenerator
  - CloakRadiusInCells
  - ConditionYellow
  - DamageDelay
  - FreeRadar
  - Immune
  - IsPowered
  - LaserFencePost
  - LowPowerPenaltyModifier
  - MaxLowPowerProductionSpeed
  - MessageDelay
  - MinLowPowerProductionSpeed
  - MultipleFactory
  - Power
  - Powered
  - PowersUpBuilding
  - PowersUpToLevel
  - Radar
  - SensorArray
  - SpeakDelay
  - TogglePower
  - Upgrades
  - Verses
  - WallBuildSpeedCoefficient
related:
  - type: system
    id: emp-pulse
  - type: system
    id: ai-base-building
  - type: event
    id: TEVENT_LOW_POWER
  - type: action
    id: TACTION_TURN_OFF_ATTACHED
  - type: action
    id: TACTION_TURN_ON_ATTACHED
---

Each house keeps two totals: its **output**, the power its structures produce, and its **drain**, the power they consume. A house whose output is below its drain is **short of power**.

## How the balance is computed

A house's totals include every structure it owns that is placed on the map and not in [limbo](/glossary/#limbo). A structure counts as soon as it is placed, so one still playing its buildup animation already adds both its output and its drain.

In a campaign game, a structure owned by a player-controlled house counts only after the player has discovered it. A base handed to the player by a trigger therefore adds nothing until the player sees it.

### Writing the assignments

This example defines a power plant, a structure that consumes power, a plug for the plant, and the `[General]` settings that govern a shortfall. The values are examples; the linked key pages give the defaults.

```ini title="rules.ini"
[MYPOWER]        ; example power plant
Power=100
Upgrades=1       ; upgrade slots this structure offers

[MYSAM]          ; example structure that consumes power
Power=-50

[MYTURBINE]      ; example plug for MYPOWER's upgrade slot
PowersUpBuilding=MYPOWER
Power=50         ; added to the host's output before damage scaling

[General]
DamageDelay=2                   ; game minutes between damage ticks while short of power
MinLowPowerProductionSpeed=0.5  ; lowest production speed a house can have
MaxLowPowerProductionSpeed=0.8  ; highest production speed a house short of power can have
LowPowerPenaltyModifier=1       ; scales the shortfall before it sets the speed
```

### What each structure contributes

A structure's output is its type's positive [`Power=`](/keys/power/#scope-buildingtype) plus the positive `Power=` of every plug installed in it, plus [`ExtraPower`](/keys/extrapower/) for each object inside an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) or [`UnitAbsorb=yes`](/keys/unitabsorb/) structure. A plug is a structure type with [`PowersUpBuilding=`](/keys/powersupbuilding/), installed into one of the host's [upgrade slots](/keys/upgrades/). The sum is multiplied by the structure's current strength as a fraction of its maximum and rounded down.

A structure's drain is the size of its type's negative `Power=`, plus the drain of its plugs. Damage does not change it.

The table shows what affects each total. A stun is an [EM pulse](/systems/emp-pulse/) disabling the structure for a set number of frames.

| | Output | Drain |
| --- | --- | --- |
| Type | positive `Power=` | size of a negative `Power=` |
| Plugs | added before scaling | added |
| Damage | scales the total, rounded down | ignored |
| Switched off | nothing | nothing |
| Stunned | unchanged | unchanged |

Damage reduces output from the first point lost. A plant with `Power=100` supplies 99 at 999 of 1000 strength, and nothing at 1 strength. Because drain does not fall with damage, a base under attack loses output while its demand holds steady, and it can become short of power.

Only switching a structure off removes it from both totals. An EM pulse does not switch a structure off, so a stunned plant keeps supplying power and a stunned consumer keeps drawing it.

### When the tally is rebuilt

A house recalculates both totals on its next update after any of these events:

- one of its structures changes strength;
- a structure finishes its buildup or is captured;
- a structure is taken off the map or changes owner;
- a plug is installed or sold;
- a structure is switched on or off;
- a human player discovers a structure.

After recalculating, the house:

- starts or stops its structures' power-dependent animations, lights, fences and cloaking fields to match the new balance;
- recalculates the speed of its factories;
- rechecks whether its radar should be up;
- rechecks its superweapons, if the house has just become short of power or just stopped being short.

## The power fraction

The **power fraction** is output divided by drain, capped at 1:

- 1 when output is at least drain;
- 1 when drain is 0, even with no output;
- 0 when output is 0 and drain is above 0;
- otherwise, output divided by drain.

A house is short of power exactly when its fraction is below 1.

:::caution[Surplus buys nothing]
The fraction is never above 1, so a house producing twice its drain is in the same state as a house producing exactly its drain. Extra output only protects against losing a plant or taking damage.
:::

## Switching a structure off

Players enter power mode with [Power Mode](/commands/togglepower/) or the sidebar's power button. The power cursor offers the toggle only over an object that passes all of these tests, in this order:

- it belongs to a player-controlled house;
- it is a structure;
- its type is [`Selectable=yes`](/keys/selectable/);
- its type is not a deployed vehicle (one with [`UndeploysInto=`](/keys/undeploysinto/) without being [`ConstructionYard=yes`](/keys/constructionyard/));
- its type is [`TogglePower=yes`](/keys/togglepower/);
- **Any of:** its type has drain, or its type is [`Powered=yes`](/keys/powered/).

Even where the cursor offers it, a switch order for a [`FirestormWall=yes`](/keys/firestormwall/) structure is refused.

A [Turn off building](/mapping/actions/taction-turn-off-attached/) trigger action or a scenario's structure record can switch off any structure whose type has drain or is `Powered=yes`. They ignore `TogglePower`. A `TogglePower=no` structure switched off this way cannot be switched back on by the player.

A stunned structure cannot be switched on.

Switching a structure off:

- removes it from both totals;
- turns off its light source;
- starts shrinking its cloaking field;
- drops its laser fences;
- stops its powered animations;
- refuses units that try to dock with it, such as harvesters at a refinery or vehicles at a repair depot;
- makes its house recheck what it can produce, if the structure is a factory;
- discharges an active firestorm wall, if it was the house's last switched-on [`GDIFirestormGenerator`](/keys/gdifirestormgenerator/).

A player-controlled house hears an announcement when a structure is switched off or back on.

While a player-controlled house's structure is switched off, a marker is drawn over it unless its cell is shrouded or fogged.

## What low power costs

Each effect below applies to every house, computer houses included, unless noted.

A computer house that is not following a map plan also tries to avoid a shortfall. It builds a power plant before a structure that would leave it short; [AI base planning](/systems/ai-base-building/#power-and-money-interventions) covers that rule and the margin it keeps.

### The structure damage tick

While a house is short of power, its structures that consume power take 1 point of damage every [`DamageDelay`](/keys/damagedelay/) game minutes. The house's timer runs whether or not the house is short, so the first tick lands anywhere up to `DamageDelay` minutes after a shortfall begins. A shortfall that ends before the next expiry costs no damage.

When the timer expires while the house is short, each structure that passes all of these tests takes 1 point of damage through [`C4Warhead`](/keys/c4warhead/):

- the house owns it;
- its strength is strictly above [`ConditionYellow`](/keys/conditionyellow/) of its maximum;
- its type has drain.

The tick ignores the on/off switch. A switched-off structure still takes damage, although it no longer adds to the drain. The drain test reads the structure's type only, so a structure whose drain comes entirely from its plugs is never damaged.

A shortfall wears a base down to `ConditionYellow` and stops there. With a 1-point tick, it destroys nothing.

The tick is not forced damage, so the warhead's [`Verses`](/keys/verses/) table applies. The 1-point hit is multiplied by the entry for the structure's armor and rounded down, so any entry below 100 percent turns the tick into nothing.

### Production

A house short of power builds more slowly. Its build time is divided by its **production speed**, which the engine sets from the house's power fraction in three steps:

1. Take the shortfall, which is 1 minus the power fraction. Multiply it by [`LowPowerPenaltyModifier`](/keys/lowpowerpenaltymodifier/) and subtract the result from 1. With the default modifier, the speed equals the power fraction.
2. Raise the speed to [`MinLowPowerProductionSpeed`](/keys/minlowpowerproductionspeed/) if it is lower.
3. While the house is short of power, lower the speed to [`MaxLowPowerProductionSpeed`](/keys/maxlowpowerproductionspeed/) if it is higher.

A house at full power keeps a speed of 1 unless its minimum is above 1.

For example, take a house with an output of 200 and a drain of 250. Its power fraction is 0.8, so its speed is 1 minus 0.2, or 0.8. With the stock rules, that is above the minimum of 0.5 and not above the maximum of 0.8, so the speed stays at 0.8. A build that takes 1,000 frames at full power, with every other multiplier at 1, takes 1,250 frames.

A speed of exactly 0 is replaced by 0.01, which makes the build 100 times as long. Keep the minimum and maximum above 0.

:::caution[The stock maximum flattens small shortfalls]
Under the stock rules, the maximum of 0.8 applies to every house short of power. Any shortfall up to 20 percent therefore makes builds a quarter longer, and a house at 99 percent power builds as slowly as one at 80 percent.
:::

The power division is one step in the build-time calculation. [How long it takes](/systems/production/#how-long-it-takes) gives the full order, including [`BuildSpeed`](/keys/buildspeed/), the country and difficulty [`BuildTime`](/keys/buildtime/) multipliers, [`MultipleFactory`](/keys/multiplefactory/), and [`WallBuildSpeedCoefficient`](/keys/wallbuildspeedcoefficient/).

Every recalculation of the balance updates the speed of the house's factories. A build in progress keeps the progress it has made and continues at the new speed, so low power slows it without restarting it, and restoring power speeds it up again. The delay between [production steps](/systems/production/#production-steps) cannot exceed 255 frames, so once a build reaches that limit a deeper shortfall slows it no further.

Low power never removes an option from the sidebar. It only slows production.

### Cash income

A [`Powered=yes`](/keys/powered/) structure that [produces cash](/systems/produce-cash/#power) pauses its income while its house is short of power, even if it draws no power itself.

### Radar

Only the local player's house can lose its radar map. The radar is up while all of these hold:

- no ion storm is running;
- the house is not short of power;
- **Any of:**
  - the scenario sets [`FreeRadar=yes`](/keys/freeradar/);
  - the house owns a [`Radar=yes`](/keys/radar/) structure that is **All of:** switched on, out of limbo, on the map, and not being sold.

In a campaign game, a radar structure the player has not discovered does not count. A player who has been given the whole map keeps the radar whatever these tests say; [observers and coach mode](/systems/observers/) covers that rule.

:::caution[A stunned radar can block a working one]
The house checks its radar structures in the order they were created and stops at the first one that passes the tests above. That structure supplies the radar only if it is not stunned. A stunned radar found first therefore keeps the map dark even while a second, working radar stands beside it.
:::

### Superweapons

A superweapon that comes from a structure is enabled while its house owns a switched-on structure that grants it, through the structure's type or through a plug. A stunned structure still counts. The weapon is disabled whenever its house is short of power.

A disabled weapon is suspended only if its type is [`IsPowered=yes`](/keys/ispowered/). Suspension stops the charge timer where it stands, replaces the status text on the sidebar cameo with a hold caption, and refuses the targeting cursor. The weapon resumes when it is enabled again.

A weapon that becomes available while its house is short of power arrives suspended, even when it is `IsPowered=no`. It resumes once power is restored.

A weapon granted by the [Add repeating special weapon](/mapping/actions/taction-full-special/) trigger action no longer needs a structure, so low power never suspends it.

:::caution[A charge-draining weapon loses its charge]
When a suspended [`UseChargeDrain=yes`](/keys/usechargedrain/) weapon resumes, its timer restarts from a full [`RechargeTime`](/keys/rechargetime/). Any shortfall that suspends it, however brief, discards all the charge it had built up. Suspending it also brings down an active firestorm wall.
:::

### Defenses

Three separate tests decide whether low power stops a defense, and they treat `TogglePower` differently.

1. **Out of service.** A structure is **operational** unless any of these holds:

   - it is switched off;
   - it is stunned;
   - its strength has reached zero;
   - **All of:** its type is `Powered=yes`, its type has drain, its type is `TogglePower=yes`, and its house is short of power.

   A structure that is not operational cannot fire. Spotlights, [laser fences](/systems/laser-fences/), sensor-array refreshes, cloak generators, and the choice of which EM pulse cannon fires use the same test.

2. **Weapons.** An operational structure still cannot fire while its house is short of power if its type is `Powered=yes` and has drain. This test ignores `TogglePower`, so a `Powered=yes`, `TogglePower=no` defense stops firing even though it remains operational.

3. **SAM tracking.** A [`SAM=yes`](/keys/sam/) launcher that is `Powered=yes` with drain stays in its ready state while its house is short of power, so it never turns toward its target.

The second test also stops a [`Charges=yes`](/keys/charges/) defense from starting a charge.

#### Overpowered defenses

Soldiers whose second weapon has an [`ElectricAssault=yes`](/keys/electricassault/) warhead can charge an [`Overpowerable=yes`](/keys/overpowerable/) structure of their own house or an allied one. Their shots at it do it no damage. An idle soldier of this kind standing next to such a structure of its own house starts charging it by itself; a player can also order a soldier to attack the structure.

The structure is **overpowered** while three or more soldiers charge it, or while at least one does, it is switched on and its house has full power. An overpowered structure counts as operational and passes the weapons test even while its house is short of power, so three chargers keep it working through a blackout. It also fires its `Secondary` weapon instead of its `Primary`.

### Fields, fences and lights

- A [`CloakGenerator=yes`](/keys/cloakgenerator/) structure that stops being operational shrinks its field ring by ring. It regrows the field the same way, out to [`CloakRadiusInCells`](/keys/cloakradiusincells/), once it is operational again and, for a `Powered=yes` type, once its house is no longer short of power. A generator that stays operational, such as one left at `Powered=no`, keeps its field through any shortfall.
- [Laser fences](/systems/laser-fences/) are rechecked at every change to the balance. A fence run is up only while the posts at both ends are operational, so low power at either end drops the whole run.
- A spotlight is neither drawn nor able to notice an intruder unless its structure is operational.
- Only a `Powered=yes` type that drains power reacts to the house's power with its attached animations. While the house is short, such a structure freezes or removes them as their [power flags](/systems/building-animations/#power) say. Each time the structure comes back into service, it starts them again. Other structures keep their animations running through a shortfall.

:::caution[A sensor array does not go dark with the rest of the base]
A power change does not remove sensor coverage. A [`SensorArray=yes`](/keys/sensorarray/) structure keeps its cells until it is taken off the map or captured. Low power matters only to an array that has not marked its cells yet: an array that finishes building while it is not operational marks nothing until any house's cloak field finishes growing while it is operational.
:::

:::caution[A TogglePower=no defense is silenced by a shortfall]
The out-of-service test spares a `TogglePower=no` structure, but the weapon test does not. Such a defense holds fire through a shortfall even though it still counts as in service.
:::

### Player feedback

The game announces low power while all of these hold:

- the house is the local player's;
- the house's announcement timer has expired;
- the house is short of power;
- it owns at least one structure of a [`BuildConst`](/keys/buildconst/) type, switched on or not.

The announcement plays the warning, shows its on-screen text for [`MessageDelay`](/keys/messagedelay/) minutes, and restarts the timer with [`SpeakDelay`](/keys/speakdelay/) minutes.

The height of the sidebar power bar grows with the size of the `Power=` rating of every structure the player owns, producers and consumers alike, leaving out plugs. A damaged or switched-off structure still adds its full rating to the height.

The bar's colors follow the real totals. While the house is short of power, the whole bar is red. Otherwise, the drain shows red, the first 100 points of surplus show yellow, and any surplus beyond that shows green.

## Scripting

The [Power Low](/mapping/events/tevent-low-power/) trigger event holds while the named house is short of power.

The [Turn off building](/mapping/actions/taction-turn-off-attached/) and [Turn on building](/mapping/actions/taction-turn-on-attached/) actions use the same switch as the power cursor, for every structure on the map that carries the trigger's tag. Turn off building affects only structures that are on, and Turn on building only those that are off.

## Parsed settings without effect

[`WorstLowPowerBuildRateCoefficient`](/keys/worstlowpowerbuildratecoefficient/) and [`BestLowPowerBuildRateCoefficient`](/keys/bestlowpowerbuildratecoefficient/) in `[General]` are read but never used. The production speed above does not depend on either.
