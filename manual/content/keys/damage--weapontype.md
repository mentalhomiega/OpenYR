---
key: Damage
scope: weapontype
label: Weapon damage
see_also: ["system:emp-pulse", "system:mind-control"]
when_omitted:
  kind: value
  value: "0"
---

`Damage` sets the damage each projectile from this weapon carries. The weapon's warhead decides what that damage does on impact.

When an object fires the weapon, a positive value is scaled by the firer's firepower modifiers before the projectile is fired: the house's firepower bias, the object's own bias, and the veteran firepower ability. A zero or negative value is used unchanged. [The shot, step by step](/systems/firing-geometry/#the-shot-step-by-step) gives the order.

An [`IsSonic=yes`](/keys/issonic/) or [`UseFireParticles=yes`](/keys/usefireparticles/) weapon fires its projectile with no damage, whatever this value is.

An [`EMEffect=yes`](/keys/emeffect/) warhead deals no damage. It uses this value as [the pulse duration in frames](/systems/emp-pulse/#firing-a-pulse) instead.

A [`MindControl=yes`](/keys/mindcontrol/) warhead deals no damage. This value is then how many objects the firer can hold under [mind control](/systems/mind-control/).
