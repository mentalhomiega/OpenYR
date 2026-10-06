---
key: ParentCountry
scope: housetype
label: Country this one is based on
see_also: [Country, Side, Color, Suffix, Prefix]
when_omitted:
  kind: value
  value: ""
  note: The country stands alone and has only its own settings.
---

`ParentCountry` makes a country start from the settings of another. The value is another country's section name, ignoring letter case. The country takes the parent's color, price and strength factors, [`Suffix=`](/keys/suffix/#scope-housetype), [`Prefix=`](/keys/prefix/), side, and whether it takes part in the multiplayer contest, and its own section then replaces whichever of these it sets. A value that names no other country is ignored and written to the debug log.

```ini title="scenario map file"
[Countries]
8=SovietsCountry

[SovietsCountry]
ParentCountry=Russians
Name=SovietsCountry
Color=DarkRed
Side=Nod
```

A country of a map's `[Countries]` is the usual place for this key. Houses that play such a country build from the tech tree of the country at the end of the `ParentCountry` chain, since the rules name only their own countries in `Owner=` lists.

The country does not take the parent's [`Multiplay=`](/keys/multiplay/), so a map country is not offered in the skirmish lobby unless it sets `Multiplay=yes` itself.
