---
key: IvanBomb.Image
summary: "The icon file shown over an object carrying a bomb this weapon planted."
see_also: [IvanBomb, "system:ivan-bombs", IvanIconFlickerRate]
when_omitted:
  kind: value
  value: "`BOMBCURS`"
---

Write the name of an `.shp` file without its extension. The icon file is read as blocks of two frames, one block for each equal share of the bomb's delay, and a block's two frames alternate every [`IvanIconFlickerRate`](/keys/ivaniconflickerrate/) frames. A last frame left over when the frame count is odd is not used. A name the game cannot load leaves the `BOMBCURS` icon, and a file with fewer than two frames shows nothing.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.Image=MYBOMBCURS
```
