---
key: CameoPCX
summary: The PCX picture drawn as an object's sidebar cameo instead of its `Cameo=` shape.
see_also: ["system:sidebar", "Cameo", "SidebarPCX"]
when_omitted:
  kind: value
  value: ""
  note: The cameo is drawn from the shape that `Cameo=` selects.
---

Write the full file name, including `.PCX`. The engine adds no extension. The file is found the way other game files are, so it can sit loose in the game folder or inside an archive. The key is read from the section of the object's [Image ID](/keys/image/), so types that share an image share a picture.

```ini title="art.ini"
[GTNK]                      ; the Image ID of the Grizzly Battle Tank
CameoPCX=GTNKICON.PCX
```

A 256-colour picture is converted through its own palette, and a 24-bit picture with three colour planes is read as it is. The picture is drawn at its own size from the top-left corner of the cameo slot, with no scaling and no transparency, and it is clipped to the sidebar strip. A picture smaller than the slot leaves the rest of the slot unchanged. Make the picture the size of the cameo shape it replaces.

The picture replaces the cameo only on the sidebar strip. The cameo shown over a structure that is being built and the cameos on the drop-ship loadout screen are still the `Cameo=` shape. The clock, the darkening and the captions are drawn over the picture as they are over a shape.

When the file is missing or cannot be read, the sidebar draws the `Cameo=` shape instead. A picture is read once, the first time it is drawn, and the result is kept for the rest of the session, so a file that was missing then stays missing until the game is restarted.

A loaded saved game reads the key again from the Image ID's section only.

`AltCameoPCX=` is not supported.
