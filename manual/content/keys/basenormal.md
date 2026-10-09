---
key: BaseNormal
summary: Whether a placed building of this type anchors its owner's later placements.
see_also: [Adjacent, EligibileForAllyBuilding, "system:base-adjacency"]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rules.ini"
[GAPOWR]
BaseNormal=no
```

`BaseNormal=no` stops a placed `GAPOWR` from serving as an [anchor](/systems/base-adjacency/) for its owner's placements, so its owner cannot use it to place another building nearby. An ally's placements do not check this key. An ally anchors on a building only through [`EligibileForAllyBuilding=yes`](/keys/eligibileforallybuilding/).

The setting belongs to the building already on the map. How far a pending building searches for an anchor is set by that pending building's [`Adjacent`](/keys/adjacent/).
