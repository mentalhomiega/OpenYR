---
title: Attach timed effects from warheads and object types
category: feature
release: 0.2.0
targets:
- type: system
  id: attach-effects
  effect: added
- type: key
  id: AttachEffect.Animation
  effect: added
- type: key
  id: AttachEffect.Duration
  effect: added
- type: key
  id: AttachEffect.TemporalHidesAnim
  effect: added
- type: key
  id: AttachEffect.SpeedMultiplier
  effect: added
- type: key
  id: AttachEffect.ArmorMultiplier
  effect: added
- type: key
  id: AttachEffect.FirepowerMultiplier
  effect: added
- type: key
  id: AttachEffect.ROFMultiplier
  effect: added
- type: key
  id: AttachEffect.Cloakable
  effect: added
- type: key
  id: AttachEffect.ForceDecloak
  effect: added
- type: key
  id: AttachEffect.DiscardOnEntry
  effect: added
- type: key
  id: AttachEffect.PenetratesIronCurtain
  effect: added
- type: key
  id: AttachEffect.Delay
  effect: added
- type: key
  id: AttachEffect.InitialDelay
  effect: added
- type: key
  id: AttachEffect.Cumulative
  effect: added
- type: key
  id: AttachEffect.AnimResetOnReapply
  effect: added
credit:
- MentalHomiega
---

In rulesmd.ini, a warhead with an `AttachEffect.Duration` attaches a timed effect to the objects it hits, and an object type with one gives the effect to itself, repeating after `AttachEffect.Delay`. While it lasts, the effect multiplies the object's speed, armor, firepower and reload time, can let it cloak or force it to uncloak, and shows an animation. `AttachEffect.Cumulative` lets hits stack, and `AttachEffect.DiscardOnEntry` removes the effect when the object enters a transport or structure. The keys follow the Ares documentation.
