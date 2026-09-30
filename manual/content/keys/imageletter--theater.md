---
key: ImageLetter
scope: theater
label: Theater image letter
see_also: [NewTheater, Theater]
when_omitted:
  kind: context-dependent
  note: "`T` for TEMPERATE, `A` for SNOW, `U` for URBAN, `D` for DESERT, `N` for NEWURBAN and `L` for LUNAR, the six built-in theaters, which keep their original settings; none for any other theater, whose artwork is then never renamed."
---

`ImageLetter` is the letter that theater-named artwork carries as the second character of its file name. In a theater lettered `T`, `GACNST.SHP` loads as `GTCNST.SHP`; in a theater lettered `A`, it loads as `GACNST.SHP`. [`NewTheater`](/keys/newtheater/) says which artwork is renamed this way.

```ini title="rules.ini"
[DESERT]
ImageLetter=D   ; GACNST.SHP loads as GDCNST.SHP
```

Only the first character of the value is read, and it is converted to upper case. An empty value keeps the letter the theater already has, so the built-in theaters always keep one.

The letter does not decide which names are renamed. A name is rewritten only when it starts with `G`, `N`, `C` or `Y` and its second letter is `A` or `T`, ignoring case. Declaring a theater with a new letter renames no other artwork.
