# True-colour sprites

A PNG sprite sheet placed where the game finds its files replaces the pixels of an SHP with the
same name. The SHP must still exist: its frame count and logical size stay in use, so animation
timing, build-up length and everything else the game decides from the SHP is
unchanged. The PNG only changes what is drawn, so it has no effect on the game state, saved games
or multiplayer synchronisation, and players in one game can have different PNGs installed.

## Names

The PNG is named after the SHP file the game loads, with the extension replaced:

| Shape file | Sheet | House-colour mask |
| --- | --- | --- |
| `GGWEAP.SHP` | `GGWEAP.png` | `GGWEAP_hc.png` |
| `TREE01.TEM` | `TREE01_TEM.png` | `TREE01_TEM_hc.png` |

Use the name of the file actually loaded, including its theater letter: the Allied war factory
draws `GGWEAP.SHP`, the generic file, when the theater has no `GTWEAP.SHP` of its own. The
`truecolour` test step below reports which name a shape has.

The game looks for the PNG the way it looks for any data file: in the game directory, the search
folders and the loaded MIX files. Shapes with the extensions `SHP`, `TEM`, `SNO`, `URB`, `UBN`,
`DES` and `LUN` can be replaced, both those the game keeps in memory with their MIX file and those
it loads on demand, such as a structure's build-up animation (`DemandLoadBuildup=yes`). The urban
construction yard, for example, builds up from `GUCNSTMK.SHP`, so its sheet is `GUCNSTMK.png`.

## Sheet layout

Each frame is a cell the size of the SHP's logical width and height. Cells run left to right, then
top to bottom. The sheet's width and height must be whole multiples of the cell size.

The sheet holds either every frame of the SHP or the first half of them. With the first half, the
remaining frames, which are the shadow frames of units and structures, are drawn from the SHP. The
sheet's row count must be the frame count divided by the column count, rounded up; a partly filled
last row is allowed.

## Shadows

A sheet that holds every frame supplies the shadows too. Where the game draws a shadow frame, each
pixel with alpha `255` halves the brightness of what is under it, exactly as a non-zero SHP shadow
pixel does; lower alpha darkens proportionally less, so a shadow can have a soft edge. The colour
of shadow pixels is ignored. A sheet with only the first half of the frames keeps the SHP's shadows.

A sheet that does not fit these rules, or that does not decode, is ignored, and the debug log gets
a line starting `TRUECOLOUR:` that names the problem. A sheet that loads also gets a log line.

## Transparency

The PNG's alpha channel decides what is drawn: `0` is transparent, `255` is opaque and values in
between are blended with what is already on screen. A PNG without an alpha channel draws every
pixel of every cell, so give sheets a transparent background. A draw the game makes 25, 50 or 75
percent translucent is applied on top of the PNG's own alpha.

## House colour

The optional mask `<stem>_hc.png` must be the same size as the sheet. Each mask pixel's brightness
(its brightest colour channel) times its alpha selects a shade: `0` leaves the sheet pixel as it
is, and any other value draws the owner's house colour, from the brightest shade at `255` to the
darkest near `1`. Those pixels are drawn through the same palette entries as the SHP's house-colour
pixels, so they match the owner's colour and lighting exactly. The sheet's alpha still applies to
them.

## Lighting

Pixels outside the mask are scaled by the light level and tint that apply to the SHP's own pixels
at the same spot, including ion storm lighting.

## Limits

These draws use the SHP as before:

- every draw on an 8-bit surface;
- shadow draws the game makes translucent or remaps;
- draws that write the per-pixel light buffer, blend by it, or draw only where it is set or clear;
- cloaked and other predator-effect draws.

A structure's PNG takes its depth from the structure's depth shape, as the SHP does, so units
behind and in front of the structure are hidden where they would be hidden with the SHP. Colours that a palette excludes from tinting are tinted in a PNG.

A sheet is decoded the first time one of its frames is drawn and is kept for the rest of the
session, also when the game frees and reloads the shape. It takes 5 bytes per sheet pixel.

## Testing

The autotest steps `truecolour <NAME.SHP>` and `truecolourdir <path>` report a shape's PNG and add
a search directory. `exportshape <NAME.SHP> <out.png>` writes a shape's frames as a sheet in this
layout, coloured with the current theater's unit palette, with a house-colour mask beside it when
the shape has house-colour pixels; it is a starting point for repainting. See
[AUTOTEST.md](AUTOTEST.md).
