---
key: GivesBounty
summary: Whether destroying this country's objects pays bounty to the destroyer's owner.
see_also: [Bounty, BountyEnablers, "system:bounty"]
when_omitted:
  kind: value
  value: "yes"
---

With `GivesBounty=no`, objects owned by this country's houses never pay [bounty](/systems/bounty/#who-pays), whatever their `Bounty.Value` keys say. Objects of other countries are not affected.

```ini title="rulesmd.ini"
[Neutral] ; example country
GivesBounty=no
```
