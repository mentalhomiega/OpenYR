---
key: Wave.Intensity
scope: weapontype
summary: "What share of its own colour a wave of this weapon adds to each pixel it covers."
see_also: ["Wave.Color", "Wave.ReverseAgainstVehicles", IsSonic, IsMagBeam, IsLaser]
when_omitted:
  kind: computed
  note: The stock intensity of the kind of wave.
---

Three whole numbers for red, green and blue, in 256ths: `256` adds the pixel's own value again at full wave strength, and `0` adds nothing. The sonic wave stock is `0,256,256`, the magnetron wave `128,0,1024` and the laser wave `0,0,0` with a constant [`Wave.Color`](/keys/wave.color/) of `64,0,96`. A weapon that sets either key draws its wave through both, and a key it leaves out takes the stock value, except that `Wave.Color` set alone drops the intensity to `0,0,0`.

```ini title="rulesmd.ini"
[MyWave] ; example WeaponType
IsMagBeam=yes
Wave.Intensity=256,0,512
```
