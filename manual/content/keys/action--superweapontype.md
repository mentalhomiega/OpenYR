---
key: Action
scope: superweapontype
label: Superweapon cursor action
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "None"
---

`Action=` decides what clicking the weapon's charged cameo does, and which click on the map fires the weapon.

- With `None`, clicking the charged cameo fires the weapon at once at cell 0,0, with no targeting step. The stock firestorm defense and hunter seeker are fired this way.
- Any other value arms targeting mode. The cursor over the map shows this action in place of the usual one, and releasing the left button fires the weapon at the cell under the pointer.
- `Custom` arms targeting mode like any other value. Its cursor comes from [`Cursor=`](/keys/cursor/) and [`NoCursor=`](/keys/nocursor/), so any number of weapons can use it.

[Aiming and the click](/systems/superweapons/#aiming-and-the-click) covers targeting mode in full.

`Nuke`, `IronCurtain`, `LightningStorm`, `ChronoSphere`, `ChronoWarp`, `ParaDrop`, `AmerParaDrop`, `PsychicDominator`, `SpyPlane`, `GeneticConverter`, `ForceShield`, `NoForceShield` and `PsychicReveal` are the Yuri's Revenge actions with superweapon cursors. `ChronoSphere` and `ChronoWarp` share a cursor, as do `ParaDrop` and `AmerParaDrop`.

`IonCannon`, `DropPod`, `ChemBomb`, `EMPulse` and `EMPulseRange` are kept from Tiberian Sun. Each shows the Yuri's Revenge cursor drawn from the same frames: the lightning storm cursor for `IonCannon` and `DropPod`, the detonate cursor for `ChemBomb`, the chronosphere cursor for `EMPulse`, and a single still frame for `EMPulseRange`.

`Custom` and `NoCustom` are the actions behind [`Cursor=`](/keys/cursor/) and [`NoCursor=`](/keys/nocursor/): a weapon with `Action=Custom` is aimed over cells it may fire at, and a weapon whose aiming rules refuse a cell reports `NoCustom` there. `NoCustom` is not a value to write in a rules file.

[`Type=`](/keys/type/#scope-superweapontype) alone decides what the weapon does when it fires; `Action=` changes only the cursor and the click. A `Type=EMPulse` weapon is the exception. Once its targeting mode is armed, its cursor is always one of the [EM pulse](/systems/emp-pulse/#em-pulse-cannon-superweapon) cursors, and an in-range click fires the first section with `Action=EMPulse`. Give a `Type=EMPulse` section `Action=EMPulse`.

The [missile crate](/systems/superweapons/#one-shot-at-a-time) also picks its weapon by this key. When some section has `Type=MultiMissile`, the crate grants the first section with `Action=Nuke`.

:::caution[An unrecognized value reads as `None`]
A misspelled value is not rejected. It becomes `None`, so clicking the charged cameo fires the weapon at cell 0,0 with no way to choose where the effect lands.
:::

:::caution[Use a value that no ordinary order uses]
A charged weapon whose value is an order such as `Attack` or `Move` fires on the player's next such order. Two sections that share a value each fire when their own cameo was clicked, but a click made without aiming fires the earlier one. [Aiming and the click](/systems/superweapons/#aiming-and-the-click) explains how a map click picks the weapon.
:::
