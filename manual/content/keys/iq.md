---
key: IQ
summary: A house's intelligence level, which turns on each computer behavior whose threshold it reaches.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "0"
---

Each automatic behavior with an entry in the `[IQ]` section turns on for a house whose `IQ` reaches that entry. Raising the value turns the behaviors on in the order of their thresholds. For example, [`RepairSell`](/keys/repairsell/) is the level that turns on [automatic repair and sell-back](/systems/repair/#when-the-computer-repairs).

Most `[IQ]` entries compare against the house's effective IQ, which starts at the value set here. Three entries are exceptions:

- [`SellBack`](/keys/sellback/) compares against the base IQ, the value set here. The match can raise the effective IQ above it (see the last paragraph), but not the base IQ.
- [`ContentScan`](/keys/contentscan/) and [`Aircraft`](/keys/aircraft/) have no effect.

With [`Paranoid`](/keys/paranoid/) on, a computer house whose `IQ` equals [`MaxIQLevels`](/keys/maxiqlevels/) makes the surviving computer houses ally with each other and declare war on the human houses when the [Destroy all of...](/mapping/actions/taction-house-destroy-all/) action defeats it, outside a campaign.

Keep the value at or below [`MaxIQLevels`](/keys/maxiqlevels/). A larger value is replaced by `1`, not by the maximum, so it turns nearly every behavior off.

Every game mode reads this value from a house the map defines. Outside a campaign, the match sets up each computer opponent with the maximum effective IQ, and the same goes for a house the computer takes over from a player who leaves. Those houses keep their base IQ: `0` for a computer the match sets up, or the map's value for a taken-over house. Unless `SellBack` is `0`, the base IQ fails [`SellBack`](/keys/sellback/) for the computer the match sets up.
