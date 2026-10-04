---
key: SidebarPCX
summary: The PCX picture drawn as a superweapon's sidebar cameo instead of its `SidebarImage=` shape.
see_also: ["system:superweapons", "system:sidebar", "SidebarImage", "CameoPCX"]
when_omitted:
  kind: value
  value: ""
  note: The cameo is drawn from the shape that `SidebarImage=` selects.
---

Write the full file name, including `.PCX`. The engine adds no extension. The file is found the way other game files are, so it can sit loose in the game folder or inside an archive.

```ini title="rules.ini"
[MyIonStrike]      ; example superweapon section
Type=IonCannon
SidebarPCX=IONCICON.PCX
```

[`CameoPCX=`](/keys/cameopcx/) describes the accepted picture formats, how the picture is drawn, and what happens when the file is missing or unreadable; a superweapon's picture follows the same rules. The charge clock and captions are drawn over the picture as they are over a shape.
