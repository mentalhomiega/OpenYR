---
command_id: CenterBase
---

Centers the view on the player's base. The view moves to the first of these that the player controls:

1. A construction yard, meaning a structure whose type sets [`ConstructionYard=yes`](/keys/constructionyard/). The primary construction yard is preferred. With several and none primary, the one listed earliest in the game's structure list is used, which is normally the one built first.
2. A vehicle whose type is listed in [`BaseUnit`](/keys/baseunit/), such as an undeployed MCV, when the player has no structure at all.
3. Otherwise the center of the player's base: the cell a [Center Base Cell](/mapping/actions/taction-center-base-cell-set/) action set, or else the cell nearest the cost-weighted average of the structures. With no such cell, the last structure in the game's structure list.

If the player has none of these, the view stays where it is.

The command ends follow mode even when the view does not move. It also works while a structure is waiting to be placed, and that structure's placement outline stays under the mouse pointer after the view moves.
