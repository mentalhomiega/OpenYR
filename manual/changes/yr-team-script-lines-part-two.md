---
title: Carry out the rest of Yuri's Revenge's team script lines
category: feature
release: 0.2.0
targets:
- type: mission
  id: TMISSION_CHRONO_PREP_ABWP
  effect: changed
- type: mission
  id: TMISSION_CHRONO_PREP_AQ
  effect: changed
- type: mission
  id: TMISSION_ATTACK_WAYPOINT_OBJECT
  effect: changed
- type: mission
  id: TMISSION_ENTER_GRINDER
  effect: changed
- type: mission
  id: TMISSION_GARRISON_STRUCTURE
  effect: changed
- type: mission
  id: TMISSION_ATTACK_BUILDING_WITH_PROPERTY
  effect: changed
- type: mission
  id: TMISSION_MOVE_TO_OWN_BUILDING
  effect: changed
- type: mission
  id: TMISSION_SCRIPT
  effect: changed
- type: mission
  id: TMISSION_DEPLOY
  effect: changed
- type: mission
  id: TMISSION_LOAD
  effect: changed
- type: key
  id: TransportsReturnOnUnload
  effect: changed
- type: key
  id: Max
  scope: teamtype
  effect: changed
- type: key
  id: RelaxedStray
  effect: added
- type: key
  id: NeedsEngineer
  effect: added
- type: enum
  id: QuarryType
  effect: changed
credit: [MentalHomiega]
---

Computer teams now chronoshift to an enemy building or to their greatest threat, attack the building on a waypoint, enter grinders, and garrison civilian buildings. Attack lines accept quarries `10` and `11`, for garrisonable and tech structures. Gather lines use `RelaxedStray` instead of `Stray`. Attack enemy building picks its structure once, so a team moves on when that structure falls, and a quarry that is not listed searches for anything. A transport keeps the cell it loaded at while its team moves on to the next line, so it can drive back after unloading. Deploy also deploys simple deployers and deployable soldiers. A TeamType that sets no `Max` has no limit. Change script now starts the new Script on the next turn.
