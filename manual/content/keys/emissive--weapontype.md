---
key: Emissive
scope: weapontype
label: Laser and electric bolt emissive strength
see_also: ["IsLaser", "IsElectricBolt", "HDRHighlights", "EmissiveDebugView"]
when_omitted:
  kind: computed
  note: "`220` for a weapon with `IsLaser=yes`, including one that also sets `IsElectricBolt=yes`. `230` for one with `IsElectricBolt=yes` alone."
---

`Emissive` on a weapon sets the strength of the beam or electric bolt the weapon draws. A weapon with [`IsLaser=yes`](/keys/islaser/) draws a beam, and a weapon with [`IsElectricBolt=yes`](/keys/iselectricbolt/) draws a bolt. A weapon with neither flag ignores the key.

The value reads the same way as the animation key, and the strength takes effect only while the emissive masks are kept, under the same conditions. For the accepted values and those conditions, see [`Emissive` for animations](/keys/emissive/#scope-animtype).

Beams from the Prism support, Ion Cannon and disk laser effects do not come from a weapon key, so this key does not set them. They draw at the laser default of `220`.

Each beam stamps its strength only along its center line, which is one pixel wide.
