---
key: HasRadialIndicator
scope: aircrafttype
label: Radius ring
when_omitted:
  kind: value
  value: "no"
---

`HasRadialIndicator=yes` draws a ring around a selected structure of this type, showing how far it reaches. Only a structure draws one. The ring appears only for the player who owns the structure, and only while the structure has power.

The radius comes from the first of these that the type has:

1. [`PsychicDetectionRadius`](/keys/psychicdetectionradius/), when it is above 0.
2. The gap radius of a [gap generator](/keys/gapgenerator/), or its super gap radius while it is charged.
3. [`CloakRadiusInCells`](/keys/cloakradiusincells/) of a sensor array or cloak generator.
4. The range of the structure's primary weapon, in whole cells.

A structure with none of these draws no ring.

The ring is an ellipse two pixels thick in the owner's color. [`ConcentricRadialIndicator=yes`](/keys/concentricradialindicator/) makes it blink and pulse instead. [`RadialColor`](/keys/radialcolor/) does not change the color.

The range ring a structure shows while it is being placed is not controlled by this key; see [the placement range ring](/systems/action-lines/#the-placement-range-ring).
