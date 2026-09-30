---
format_id: game-fnt
title: GAME.FNT Unicode font
summary: Holds the one bitmap font Yuri's Revenge draws all of its text with, indexed by Unicode code point.
kind: binary
source_files:
  - code/bitfont.h
  - code/bitfont.cpp
  - code/bitfontdata.cpp
  - code/init.cpp
extensions:
  - .FNT
role: text
---

The game draws its text with `GAME.FNT` when the file is present and is a complete font of the layout below. Every text style then uses it. Without it, or when it cannot be read, the game falls back to the older fonts: `12METFNT.FNT`, `KIA6PT.FNT`, `6POINT.FNT`, `EDITFNT.FNT`, `8POINT.FNT` and `GRAD6FNT.FNT`.

## Layout

All numbers are little-endian.

| Offset | Size | Contents |
| --- | --- | --- |
| 0 | 4 | The characters `fonT` |
| 4 | 4 | Width of the widest glyph, in pixels |
| 8 | 4 | Bytes in one row of a glyph |
| 12 | 4 | Glyph height, in pixels |
| 16 | 4 | Line count, not used |
| 20 | 4 | Number of glyphs |
| 24 | 4 | Bytes in one glyph |
| 28 | 131072 | One 16-bit entry for each code point from `U+0000` to `U+FFFF` |
| 131100 | glyph count × glyph size | The glyphs |

A table entry of `0` means the font has no glyph for that code point. Any other entry is the glyph's position, counting from `1`. An entry larger than the glyph count is treated as `0`.

Each glyph starts with its width in pixels. Its rows follow, top first, one bit per pixel with the most significant bit leftmost. A set bit is drawn and a clear bit is left transparent. A glyph's size must hold its width byte and all of its rows; a font whose sizes do not fit, or whose file is shorter than its header says, is not used.

## Drawing

Each character advances the print position by its glyph's width plus one pixel. A character the font lacks is drawn and measured as a question mark, and a character with neither is skipped.

Set bits take the text's colour. When the text has a shadow, the glyph is drawn first in the shadow colour one pixel down and to the right.
