# True-colour sprites

A PNG sprite sheet placed where the game finds its files replaces the pixels of an SHP with the
same name. The SHP's frame count and logical size stay in use, so animation timing, build-up
length and everything else the game decides from the SHP is unchanged. The PNG only changes what
is drawn, so it has no effect on the game state, saved games or multiplayer synchronisation, and
players in one game can have different PNGs installed. A sheet whose SHP does not exist adds new
art instead; see [Sprites without an SHP](#sprites-without-an-shp).

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

## Sprites without an SHP

When the game asks for a shape that none of its loaded MIX files holds and it finds a sheet with
that shape's PNG name, it makes a shape from the sheet and draws the sheet's frames in its place.
A new soldier with `Image=TRUEMAN` needs only `TRUEMAN.png`. The made shape has the sheet's cell
size and the frame count its layout gives. Its pixels are all transparent, and each frame's
rectangle covers the pixels of its cell that are not fully transparent, as an SHP frame's
rectangle covers its visible pixels.

By default the sheet is one row of square cells as tall as the sheet; a sheet whose width is not a
multiple of its height is a single cell. The shape has twice as many frames as the sheet has
cells: the second half are the shadow frames that units, soldiers and structures draw, and they
are empty. A file named after the sheet with `.ini` added, such as `TRUEMAN.png.ini`, can set other
values in a `[Sheet]` section:

| Key | Sets | Default |
| --- | --- | --- |
| `FrameWidth=` | The cell width in pixels | `FrameHeight` when the sheet's width is a multiple of it, otherwise the sheet's width |
| `FrameHeight=` | The cell height in pixels | The sheet's height |
| `Frames=` | The shape's frame count | Twice the number of cells |

The cells then follow [Sheet layout](#sheet-layout): the sheet holds either every frame or the
first half. To give the sprite [shadows](#shadows), put the shadow frames after the others and
set `Frames=` to the number of frames in the sheet. An animation whose art sets neither
`Shadow=yes` nor `End=` plays every frame of its shape, so an animation without shadows needs
`Frames=` set to its number of frames.

The cell's centre is drawn at the object's position, as an SHP frame's centre is. The GI's feet
are 2 pixels below the centre of its frames, so place a soldier's feet there.

A sheet that does not decode or does not fit its layout makes no shape, so the game behaves as if
the art were missing, and the debug log gets a `TRUECOLOUR:` line that names the problem.

Game rules can read a shape's frame count and size; an animation without `End=`, for example,
takes its length from the frame count. Every player in a multiplayer game therefore needs the same
sheet and `.ini` file for a sprite without an SHP.

A sheet named after a theater variant that the game tries first stands in for that missing file.
With `GTWEAP.png` and no `GTWEAP.SHP`, the temperate Allied war factory gets a shape made from
`GTWEAP.png` and no longer falls back to `GGWEAP.SHP`.

Art the game reads from disk when it first needs it still needs an SHP. This covers a structure's
build-up with `DemandLoadBuildup=yes` and an animation with `DemandLoad=yes`. Where the game
draws the SHP itself, as listed below, a sprite without an SHP draws nothing.

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

The autotest steps `truecolour <NAME.SHP>` and `truecolourdir <path>` report a shape's PNG, with
`png-only` for a shape made from a sheet, and add a search directory. `exportshape <NAME.SHP> <out.png>` writes a shape's frames as a sheet in this
layout, coloured with the current theater's unit palette, with a house-colour mask beside it when
the shape has house-colour pixels; it is a starting point for repainting. See
[AUTOTEST.md](AUTOTEST.md).
