---
key: Wave.Color
scope: weapontype
summary: "What a wave of this weapon adds to the colour of the pixels it covers."
see_also: ["Wave.Intensity", "Wave.ReverseAgainstVehicles", IsSonic, IsMagBeam, IsLaser]
when_omitted:
  kind: computed
  note: "The stock tint of the kind of wave: a sonic wave brightens green and blue, a magnetron wave brightens red and strongly blue, and a laser wave adds red and a little blue."
---

The weapon's sonic, magnetron or laser wave adds these red, green and blue amounts, each from `-255` to `255` and written in decimal or as hexadecimal with a trailing `h`, to every pixel it covers whatever the pixel's own colour. The amounts are scaled by the strength of the wave at the pixel, which is a little under one for a sonic wave and fades out over a laser wave's life. A negative amount darkens. Setting `Wave.Color` drops the stock intensity unless [`Wave.Intensity`](/keys/wave.intensity/) is set as well.

```ini title="rulesmd.ini"
[MyWave] ; example WeaponType
IsSonic=yes
Wave.Color=0,64,0
```
