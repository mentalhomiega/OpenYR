---
key: IsHouseColor
summary: "Draws the weapon's laser in its firer's house laser color."
see_also: [IsLaser, LaserInnerColor, LaserOuterColor, LaserOuterSpread, LaserDuration]
when_omitted:
  kind: value
  value: "no"
---

An [`IsLaser=yes`](/keys/islaser/) weapon with `IsHouseColor=yes` draws its beam in the firer's house laser color, as a thick beam made of parallel lines. The beam has no line of its own along its path. [`LaserInnerColor`](/keys/laserinnercolor/) and [`LaserOuterColor`](/keys/laseroutercolor/) are ignored. The glow's spread still comes from [`LaserOuterSpread`](/keys/laserouterspread/), as it does for any laser.

## The thick beam

The beam is drawn as pairs of lines, one line of each pair on either side of the path, each pair one pixel further out than the last. How many pairs depends on what fired:

| Fired by | Pairs |
| --- | --- |
| A vehicle, infantry or aircraft | 2 |
| A [`PrismType`](/keys/prismtype/) structure | 3 |
| A `PrismType` structure with other towers [supporting](/systems/prism-towers/#recruiting-support) it | 5 |
| A prism tower's support beam to the tower it supports | 3 |
| Any other structure | 1 |

The first pair is half the house laser color, and each pair after it is half the one before. Drawing stops early when all three channels of the next pair would be below 8. A supported tower's beam starts brighter: its first pair is twice the house color (each channel capped at 255), its second the house color, and the pairs after that halve from there.

Each line adds its color to the pixels beneath it, in proportion to the beam's strength. The strength falls evenly from full to nothing over [`LaserDuration`](/keys/laserduration/) frames, so the beam fades out. This holds only at a high enough frame rate. While the rate is below [`DetailMinFrameRateNormal`](/keys/detailminframeratenormal/#scope-global-rules), or at the lowest [detail level](/keys/detaillevel/#scope-client-settings), each line is a plain solid line instead. Its color is the pair's color moved toward white by the beam's strength, and drawing stops when all three channels would fall below 64.

The house laser color is the house color scaled to a brightness magnitude of 240, with each channel capped at 255. Channels below 96 are then set to zero, and the result is scaled to magnitude 240 again. A black house color therefore gives 138 on each channel.

```ini title="rulesmd.ini"
[MyPrismShot] ; example Weapon
IsLaser=yes
IsHouseColor=yes
```
