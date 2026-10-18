---
key: UnderRoofDoorAnim
summary: The shape drawn beneath a structure's factory door while a jumpjet or balloon-hover unit leaves through the roof.
see_also: ["UnderDoorAnim", "DoorAnim", "DoorStages", "DeployingAnim", "JumpJet", "BalloonHover", "WeaponsFactory"]
when_omitted:
  kind: value
  value: ""
  note: No roof door shape is loaded, and no under-door shape is drawn for a unit that leaves through the roof.
---

`UnderRoofDoorAnim` names a shape that a structure draws while it unloads a unit that leaves through the roof. A unit leaves through the roof when its type sets [`JumpJet=yes`](/keys/jumpjet/) or [`BalloonHover=yes`](/keys/balloonhover/). Write the filename without its extension; the engine loads `<value>.SHP`. The name gets the same theater letter rewrite that [`DoorAnim`](/keys/dooranim/) describes.

The type of the unit being unloaded decides which shape is drawn. A structure draws `UnderRoofDoorAnim` for a unit that leaves through the roof and [`UnderDoorAnim`](/keys/underdooranim/) for any other unit. When `UnderRoofDoorAnim` is omitted, no under-door shape is drawn for a roof exit, even if `UnderDoorAnim` is set.

The shape is drawn, framed and lit as [`UnderDoorAnim`](/keys/underdooranim/) describes.
