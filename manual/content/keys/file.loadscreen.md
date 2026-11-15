---
key: File.LoadScreen
summary: The shape drawn as the loading screen of a skirmish or multiplayer match for a player of the country.
see_also: ["system:loading-screens", "File.LoadScreenPAL", "LoadScreenText.Name", "LoadScreenText.SpecialName", "LoadScreenText.Brief", "LoadScreenText.Color", "File.Flag"]
when_omitted:
  kind: computed
  note: The picture the stock rules give the country's position in `[Countries]`, such as `LS800USTATES.SHP` for the first. A country that has no stock position and sets no file shows no Yuri's Revenge loading screen.
---

Write the file name with its `.SHP` extension. The shape is the 800 by 600 backdrop of the loading screen, which the game scales to the window. The country's name, its special unit, its description and the player rows are printed over it at the stock positions, and the map's preview is drawn in its box.

```ini title="rules.ini"
[Guild1]
File.LoadScreen=ls800haihead.shp
File.LoadScreenPAL=mplshh.pal
LoadScreenText.Name=Name:Haihead
```

The screen is drawn for the country of the local player, or for the observer picture when the player observes. A file that cannot be found, or a palette that cannot be read, drops the Yuri's Revenge loading screen and shows the older loading screen instead.
