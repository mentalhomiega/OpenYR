---
title: Read seven object type keys from rules
category: feature
release: 0.2.0
targets:
- type: key
  id: NoShadow
  effect: added
- type: key
  id: PreventAttackMove
  effect: added
- type: key
  id: VoiceSpecialAttack
  effect: added
- type: key
  id: StupidHunt
  effect: added
- type: key
  id: TiltCrashJumpjet
  effect: added
- type: key
  id: HasTurretTooltips
  effect: added
- type: key
  id: RejoinTeamIfLimboed
  effect: added
credit: [MentalHomiega]
---

`NoShadow=yes` stops a vehicle or aircraft drawing its voxel shadow on the ground.

`PreventAttackMove=yes` keeps a ground move or attack order from becoming an attack-move. Before, a move or attack order on the ground became an attack-move for every vehicle, infantryman and aircraft when an attack-move was armed, its key was held, or Control and Shift were held. Now only a type with a primary weapon takes that conversion, and aircraft never do.

`VoiceSpecialAttack` supplies the voice for guard, area guard, patrol and sabotage orders. Before, those orders played `VoiceMove`. Now they play a random `VoiceSpecialAttack` entry, or nothing when that list is empty.

`StupidHunt=yes` stops an infantryman or vehicle searching for a target during its hunt mission. In a campaign game, an object owned by a computer player then walks to its house's starting cell, as the original game does. Other objects with the key stay where they are.

`RejoinTeamIfLimboed=yes` makes a parasite that fires a LimboLaunch weapon at an infantryman rejoin its team when it comes out of its victim. Before, the key had no effect.

`TiltCrashJumpjet` and `HasTurretTooltips` are read and stored. They have no effect in this build.
