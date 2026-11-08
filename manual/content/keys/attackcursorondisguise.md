---
key: AttackCursorOnDisguise
summary: Whether the attack cursor shows over an enemy whose disguise fools the player.
see_also: [Disguise, PermaDisguise, AttackCursorOnFriendlies]
when_omitted:
  kind: value
  value: "no"
---

With `AttackCursorOnDisguise=no`, pointing at an enemy whose disguise fools the player gives the cursor an allied object would give, not the attack cursor. Holding the force-fire key still gives the attack cursor. A disguise sensor over the enemy's cell stops the disguise fooling the player. Of the stock units, the setting affects only the Mirage Tank, because the stock spy is `PermaDisguise=yes`.

```ini title="rules.ini"
[General]
AttackCursorOnDisguise=yes ; the stock setting: disguised enemies can be clicked to attack
```

The attack cursor still appears over a disguised enemy when any of these holds:

- the enemy's type is [`PermaDisguise=yes`](/keys/permadisguise/);
- the enemy is a Mirage Tank in the blink after its last shot;
- the enemy is a Mirage Tank and the weapon that would fire at it is [`TerrainFire=yes`](/keys/terrainfire/), so it could attack the tree the tank looks like;
- the selected object can attack friends, through [`AttackFriendlies=yes`](/keys/attackfriendlies/) or [`AttackCursorOnFriendlies=yes`](/keys/attackcursoronfriendlies/).

Stock Yuri's Revenge sets `yes`, so the setting changes nothing in a stock game. A rules file that leaves it out gets `no`.
