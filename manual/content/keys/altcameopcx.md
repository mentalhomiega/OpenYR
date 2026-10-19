---
key: AltCameoPCX
summary: "The PCX picture drawn as an object's sidebar cameo in place of its alternate cameo."
see_also: [CameoPCX, AltCameo, "system:sidebar"]
when_omitted:
  kind: value
  value: ""
  note: The alternate cameo is the `AltCameo=` shape, or `Cameo=` when there is none.
---

The picture replaces the cameo only while the alternate cameo would be shown, as [`AltCameo=`](/keys/altcameo/) describes: after the local player has infiltrated a barracks or war factory, for a `Trainable=yes` type that factory makes. It is read from the section of the object's [Image ID](/keys/image/). Write the full file name, including `.PCX`; the picture is found and drawn as [`CameoPCX=`](/keys/cameopcx/) describes.

```ini title="art.ini"
[GTNK]                      ; the Image ID of the Grizzly Battle Tank
CameoPCX=GTNKICON.PCX
AltCameoPCX=GTNKALT.PCX
```

An object that has `AltCameoPCX=` draws that picture while the alternate cameo is shown, whether or not it has `AltCameo=`. An object without it draws its `CameoPCX=` picture, or the cameo shape when there is none.
