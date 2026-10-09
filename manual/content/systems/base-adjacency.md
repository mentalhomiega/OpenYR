---
title: Base placement and adjacency
summary: "Whether a player may place a building at a site, based on the anchor buildings and owned wall cells near it."
category: buildings-economy
keys:
  - Adjacent
  - BaseNormal
  - EligibileForAllyBuilding
related:
  - type: format
    id: spawn-ini
---

A player can place a building only near an **anchor**: a building already on the map that the new one may be built against. The anchor must be one of the player's own buildings or, when the match allows [building off an ally](#building-off-an-ally), an ally's building of a type that allows it. Where no anchor is in range, the placement cursor shows the site as blocked and clicking there does not place the building.

The rule applies only to buildings a player places from the sidebar. Computer houses place their buildings without it. An upgrade clicked onto a building that accepts it is also exempt.

Only the placing player's machine runs the check. The other machines in the match accept the placement without repeating it.

## Anchor eligibility

[`BaseNormal`](/keys/basenormal/) and [`EligibileForAllyBuilding`](/keys/eligibileforallybuilding/) decide whether a building already on the map can anchor. [`Adjacent`](/keys/adjacent/) belongs to the building being placed.

| Setting | Read from | What it controls |
| --- | --- | --- |
| [`BaseNormal`](/keys/basenormal/) | The type of a building already on the map | Whether a player's own building of that type anchors that player's placements |
| [`EligibileForAllyBuilding`](/keys/eligibileforallybuilding/) | The type of a building already on the map | Whether an ally's building of that type anchors a placement, when the match allows building off an ally |
| [`Adjacent`](/keys/adjacent/) | The BuildingType being placed | How far its foundation searches for an anchor |

```ini title="rules.ini"
[GAPOWR]
BaseNormal=no ; a placed GAPOWR cannot anchor later placements
Adjacent=5    ; a GAPOWR being placed searches this far for an anchor
```

`Adjacent` is not a radius around an existing building. Raising it on one BuildingType lets that type be placed farther from its anchor and changes nothing for other types.

Laser fence types anchor a player's own placements like any other building unless their section sets `BaseNormal=no`. Stock rules set it for the fence post `NAPOST` but not for the fence section `NAFNCE`.

## Placement decision order

The search covers the block of cells that extends `Adjacent` + 1 cells beyond each edge of the pending foundation, corners included. Cells inside the foundation itself are not examined. With `Adjacent=5`, for example, the search reaches six cells out from each side. A negative value leaves no cells to examine, so the check always fails.

The adjacency check passes when any examined cell passes one of these tests:

- **Placing any building:** the cell holds an eligible anchor: one of the placing player's buildings whose type has `BaseNormal=yes`, or, when the match allows building off an ally, an ally's building whose type has `EligibileForAllyBuilding=yes`.
- **Placing a wall:** the cell is owned by the placing player, whether or not a building stands there. A player owns the cells under their walls; [walls and gates](/systems/walls-and-gates/#who-owns-a-wall) explains how wall cells get an owner. This lets a wall run extend from the end of an earlier wall.

If no examined cell passes either test, the check fails and the building cannot be placed there.

:::note[Adjacent zero still permits contact]
Because the search reaches one cell beyond the `Adjacent` value, `Adjacent=0` still finds an anchor that touches the foundation, including diagonally.
:::

## Building off an ally

`BuildOffAlly=yes` in the [launch file](/formats/spawn-ini/), or the Build Off Ally checkbox in the skirmish lobby, lets a player's placements use an ally's buildings as anchors, but only those whose type sets [`EligibileForAllyBuilding=yes`](/keys/eligibileforallybuilding/). The stock rules set that flag only on the two construction yards, so with the stock rules allies can place next to each other's construction yards and no other structure.

The test reads only the alliance held by a building's owner. A building anchors when its owner counts the placing player as an ally, even if the placing player does not count its owner. The alliance does not need to run both ways.

An ally's building does not need `BaseNormal=yes`, because that key applies only to a player's own buildings.

The wall cell test is unchanged: it accepts only cells the placing player owns, so an ally's wall cell does not pass it. An ally's wall building anchors like any other ally's building and needs `EligibileForAllyBuilding=yes`.

Like the rest of the check, `BuildOffAlly` affects only human players.
