---
key: PsiWarning
summary: Shows the animation only to players whose own psychic detectors cover its cell.
see_also: ["NukeTakeOff", "NukeSilo", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `PsiWarning=yes`, the game shows the animation to a player only when both conditions hold:

- The player is not allied with the animation's owner. A player never sees its own warnings. An animation with no owner passes this test for every player.
- One of the player's own structures with a [`PsychicDetectionRadius=`](/keys/psychicdetectionradius/#scope-buildingtype) above `0` is powered and has the cell within that many cells. The distance runs from the structure's center to the center of the cell. Detectors that belong to the player's allies do not count.

The check runs every game frame, so a detector that is built, unpowered or destroyed changes the result on the next frame. Without this key, the animation is shown to every player as usual.

The stock `PSIWARN` animation sets this key. A nuke silo places it on its target when a launch begins. [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile) covers when it is removed.
