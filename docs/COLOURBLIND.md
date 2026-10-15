# Colour-blind correction

The colour-blind correction is one 3x3 matrix on linear RGB for each deficiency. The composite pass
applies it to the whole picture, so the units, the terrain and the interface all change together. The
settings are [`ColorBlindMode`](/keys/colorblindmode/) and [`ColorBlindStrength`](/keys/colorblindstrength/).
[`Brightness`](/keys/brightness/) and [`Gamma`](/keys/gamma/) use the same hook.

## Where the matrices are

`code/colourblind.cpp` holds the matrices and `ColorBlind_Matrix()` builds the correction for a mode and
strength. `Video_Apply_Colour_Settings()` (`code/video.cpp`) passes the result to
`Video_Set_Colour_Adjust`, which sets the adjustment the composite reads. The unit test `colourblind`
(`tests/colourblind/colourblind_test.cpp`) checks the values.

## The maths

The simulation matrix `S` for each deficiency is the severity 1.0 matrix of Machado, Oliveira and
Fernandes, "A Physiologically-based Model for Simulation of Color Vision Deficiency", IEEE Transactions on
Visualization and Computer Graphics 15(6), 2009. Each row sums to 1, so white stays white.

The correction is the Daltonize form `M = I + K (I - S)`. `I - S` is the part of each colour the viewer
cannot see. `K` moves that part into the channels the viewer can still see:

| Mode | `K` (rows are the output channels, columns the lost part of red, green and blue) |
| --- | --- |
| Protanopia | `0 0 0`, `0.7 1 0`, `0.7 0 1` |
| Deuteranopia | `0 0 0`, `0.7 1 0`, `0.7 0 1` |
| Tritanopia | `1 0.7 0`, `0 0 0`, `0 0.7 1` |

The `K` values are the ones the common open-source Daltonize implementations use. They are not taken from
a paper, and nobody has checked them against a colour-blind observer. Judge them on a display before you
rely on them.

`M` at strength `s` is `(1 - s) I + s M`. At `s = 0` the result is the identity exactly, so the picture is
unchanged. Strength is clamped to 0 to 1.

## Where it applies

- The composite pass reads the matrix on SDR and HDR output alike, because the adjustment routes frames
  through the scene target whenever it is not the identity.
- Screenshots are taken from the window. In SDR the window already holds the adjusted picture, so the
  screenshot includes the correction.
- It needs Direct3D 11. On another interface the composite program does not exist and the picture is
  drawn without the correction.

## Limits

- The correction is a simulation-based daltonize. It moves colours apart, but it does not guarantee that
  two house colours are easy to tell apart. Check the house colours that matter in the game.
- Values outside the display's range are clipped after the matrix. A strong correction can clip greens to
  black, as it does for green units under protanopia.
