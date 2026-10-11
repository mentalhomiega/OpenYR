---
key: Sonic
summary: Makes a warhead drive a parasite out of the vehicle or soldier it is eating.
see_also: [IsSonic, Parasite, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

A hit from a warhead with `Sonic=yes` drives out any parasite eating the vehicle, ship, soldier or aircraft it hits. The hit still does its normal damage. The firer then drops its target, so a firer that is still in range has to be given a new order. The dolphin's `SonicZap` weapon uses this to free a ship from a giant squid.

```ini title="rulesmd.ini"
[MySonicWarhead] ; example Warhead
Sonic=yes
```

The parasite comes out as it does when its victim dies, onto the victim's cell or the nearest cell it can stand on. [Parasites](/systems/parasites/) covers the cases where it is lost instead.

A player's selected unit also shows the attack cursor over an allied vehicle, ship, soldier or aircraft with a parasite inside when the weapon it would fire there has a `Sonic=yes` warhead. Without it, the cursor shows over an allied object only with the force-fire key held, or for a unit whose type sets [`AttackCursorOnFriendlies=yes`](/keys/attackcursoronfriendlies/) or [`AttackFriendlies=yes`](/keys/attackfriendlies/).

Despite the name, the key does not make a weapon fire a sonic wave. That is [`IsSonic=yes`](/keys/issonic/) on the weapon.
