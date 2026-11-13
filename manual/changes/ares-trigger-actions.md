---
title: Run Ares's trigger actions 146 to 149 and its Firestorm actions
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_AUXILIARY_POWER
  effect: added
- type: action
  id: TACTION_KILL_DRIVERS_OF
  effect: added
- type: action
  id: TACTION_SET_EVA_VOICE
  effect: added
- type: action
  id: TACTION_SET_GROUP_ID
  effect: changed
- type: action
  id: TACTION_ACTIVATE_FIRESTORM
  effect: changed
- type: action
  id: TACTION_DEACTIVATE_FIRESTORM
  effect: changed
- type: key
  id: FirestormTriggerHumans
  effect: added
credit: [MentalHomiega]
---

A map written for Ares now runs the actions it names at 146 to 149: Auxiliary Power, Kill Drivers Of, Set EVA Voice and Set Group. Set Group moved from 148 to 149. The extra OpenTS actions that stood at 146 to 161 now follow from 150, in the same order with Set Group taken out, so that no OpenTS action shares a number with an Ares one. Maps and saves that use an extra action by its old number must use the new one.

Activate and Deactivate Firestorm Defense now work as Ares documents them: for houses a computer controls, with no charge needed or spent. A raised wall drops when the house runs short of power. A player's house is left alone unless `FirestormTriggerHumans=yes`.
