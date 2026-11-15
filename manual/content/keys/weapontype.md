---
key: WeaponType
summary: The WeaponType a missile silo launches, read only from the first and sixth entries of the superweapon list.
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A missile silo launching a repeating `Type=MultiMissile` or `Type=ChemMissile` superweapon takes the missile's projectile, warhead and maximum speed from the WeaponType this key names, in the section the caution below identifies. It ignores that WeaponType's `Damage=` and gives the missile [a fixed strength of 200](/systems/superweapons/#multi-missile-and-chem-missile).

No other use of a superweapon reads this key. A one-time missile, granted by a crate or by [Add 1-time special weapon](/mapping/actions/taction-1-special/), is built from the weapon `NukePayload` or `ChemLauncher` instead, according to its behavior. The [Nuke strike](/mapping/actions/taction-multi-missile/) and [Chem-missile strike](/mapping/actions/taction-chem-missile/) trigger actions read the same two weapons.

:::caution[The silo reads this key by list position]
A silo does not read the `WeaponType=` of the section that ordered the launch. It reads the section at a fixed position in `[SuperWeaponTypes]`: the first entry for every `Type=MultiMissile` launch and the sixth for every `Type=ChemMissile` launch. The shipped list puts its multi missile and chem missile sections at those positions.

A custom `Type=MultiMissile` section added after the shipped seven therefore charges and fires normally, but its missile uses the first entry's `WeaponType=`. [Declaring a superweapon](/systems/superweapons/#declaring-a-superweapon) gives the full order to keep.
:::

:::danger[Give the first and sixth sections a WeaponType]
Set `WeaponType=` in the first section of `[SuperWeaponTypes]` when any section uses `Type=MultiMissile`, and in the sixth when any section uses `Type=ChemMissile`. If the section a launch reads sets none, the game crashes when the silo launches the missile.
:::
