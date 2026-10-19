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

`StupidHunt=yes` stops an infantryman or vehicle searching for a target during its hunt mission, so it stays where it is. The walk toward a starting cell that the original game makes such an object take is not implemented.

`TiltCrashJumpjet`, `HasTurretTooltips` and `RejoinTeamIfLimboed` are read and stored. They have no effect in this build.
