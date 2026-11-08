---
key: RoofDeployingAnim
summary: The shape drawn in place of a structure's main artwork while a jumpjet or balloon-hover unit leaves through the roof.
see_also: ["DeployingAnim", "UnderRoofDoorAnim", "UnderDoorAnim", "DoorAnim", "JumpJet", "BalloonHover", "WeaponsFactory"]
when_omitted:
  kind: value
  value: ""
  note: No roof shape is loaded, and the structure keeps its own artwork while a unit leaves through the roof.
---

`RoofDeployingAnim` names a shape that a structure draws in place of its main artwork while it unloads a unit that leaves through the roof. A unit leaves through the roof when its type sets [`JumpJet=yes`](/keys/jumpjet/) or [`BalloonHover=yes`](/keys/balloonhover/). Write the filename without its extension; the engine loads `<value>.SHP`. The name gets the same theater letter rewrite that [`DoorAnim`](/keys/dooranim/) describes.

A roof exit never uses [`DeployingAnim`](/keys/deployinganim/). Without `RoofDeployingAnim`, the structure keeps its own artwork for that exit, so the front door art does not show.

Lay the file out like the main artwork, with the healthy frames first and the damaged frames after them. The frame drawn never goes past half the file's frame count. The shape is drawn 40 pixels higher than the structure's base position, and the type's [`NormalZAdjust`](/keys/normalzadjust/) is left out of that draw.

[`UnderRoofDoorAnim`](/keys/underroofdooranim/) is drawn beneath the door for the same exit.
