---
title: Add Classic and Modern control schemes with press, double press, hold and held bindings
category: feature
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
- type: command
  id: AttackMove
  effect: added
- type: command
  id: NextSidebarTab
  effect: added
- type: command
  id: ForceFire
  effect: added
- type: command
  id: ForceMove
  effect: added
- type: command
  id: SelectModifier
  effect: added
- type: command
  id: QueueMove
  effect: added
- type: command
  id: TeamCenter_1
  effect: changed
credit: [MentalHomiega]
---

The keyboard page now lists its commands by group, with a search box and a category filter. Each command has a primary and a secondary key, and each key has a trigger: press, double press, hold or held. A control scheme, Classic or Modern, sets the defaults. Classic keeps the Yuri's Revenge keys. Modern adds attack-move on A, waypoint mode on V, arrow keys to scroll, Home for the base, F10 for the menu, Tab for the sidebar tabs, and a double press of a group number to centre on the group.

The side mouse buttons and the mouse wheel can be bound. A bound wheel step replaces the built-in zoom or sidebar scroll for that step.

Attack-move is new. The AttackMove command arms the next left click on the ground, and Control with Shift and a left click on the ground gives the same order. The units travel to the cell and engage an enemy they reach on the way.

Existing keyboard files still load. A version 2 file gains the Control, Alt, Shift and Q holds that version 3 adds, and nothing else changes.
