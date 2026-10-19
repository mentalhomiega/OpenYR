---
key: MigAttackCursor
summary: Shows the airstrike cursor over a structure that allows C4, when this weapon is the one an infantryman would fire at it.
see_also: ["CanC4", "InvisibleInGame", "C4"]
when_omitted:
  kind: value
  value: "no"
---

With `MigAttackCursor=yes`, a selected infantryman shows the airstrike cursor over a structure that allows C4, when this weapon is the one it would select for that structure. A structure allows C4 when its type sets [`CanC4=yes`](/keys/canc4/) and does not set [`InvisibleInGame=yes`](/keys/invisibleingame/). A click on that structure orders an ordinary attack.

Any other target keeps the attack cursor. So does a structure that does not allow C4, and so does a target for which the infantryman would select a different weapon.

The key changes only the action an infantryman reports for that structure, which the cursor and the clicks on it use. It does not change the weapon that fires.
