---
key: Warhead
scope: animtype
label: Animation warhead
see_also: ["Damage", "DamageRadius", "ExpireAnim"]
when_omitted:
  kind: value
  value: none
---

The warhead delivers an animation's [`Damage`](/keys/damage/#scope-animtype): while an ordinary animation plays, and when a thrown animation, one with [`Bouncer=yes`](/keys/bouncer/) or [`IsMeteor=yes`](/keys/ismeteor/#scope-animtype), lands. Landing deals damage in two ways:

- Each object in the landing cell within [`DamageRadius`](/keys/damageradius/#scope-animtype) of the landing point takes the damage.
- If the animation names an [`ExpireAnim`](/keys/expireanim/#scope-animtype), a blast of the same damage goes off at the landing point. There is no blast when the animation lands in water, unless it lands on a bridge.

The warhead's armor multipliers and its [`Spread`](/keys/spread/#scope-warheadtype) decide what the damage is worth against each target. Its [`Bright`](/keys/bright/#scope-warheadtype) flag makes the blast throw a flash of light.

With no warhead, landing deals no damage and throws no light. The animation still flies, starts its expire animation and plays its sounds, and it still craters or spreads tiberium where its other settings ask for it.

An animation that is not thrown and has `Damage` above `0` deals its damage around its position through [`FlameDamage2`](/keys/flamedamage2/), or [`C4Warhead`](/keys/c4warhead/) for the animation named `INVISO`, with no attacker, as Yuri's Revenge does. In Ares rules, which declare their own armor types in `[ArmorTypes]`, it uses this warhead instead when the animation names one, as Ares does.

In Ares rules that damage also counts the animation's owner as the attacker, so a warhead with [`AffectsAllies=no`](/keys/affectsallies/) spares the owner's allies and [`AffectsOwner`](/keys/affectsowner/#scope-warheadtype) decides for the owner's own objects. The animation of an object's own [`AttachEffect.Animation`](/keys/attacheffect.animation/) belongs to the object's current owner. An animation with no owner hurts every house.
