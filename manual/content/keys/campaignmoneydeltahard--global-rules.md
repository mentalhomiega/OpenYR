---
key: CampaignMoneyDeltaHard
scope: global-rules
summary: "The credits a player-controlled campaign house gains on Hard difficulty at the start of a mission."
see_also: [CampaignMoneyDeltaEasy]
when_omitted:
  kind: value
  value: "-5000"
---

`CampaignMoneyDeltaHard` is a Yuri's Revenge key, read from `[General]`. When a campaign mission loads, each house whose map section has `PlayerControl=yes` starts with its `Credits` (times 100) plus this number if the campaign difficulty chosen for the player is Hard. Normal adds nothing. A result below zero becomes zero.

Only the player's difficulty counts, not the computer's. A skirmish or multiplayer game, and a house without `PlayerControl=yes`, get no delta. It applies once, when the house is read; money granted later is not changed. [`CampaignMoneyDeltaEasy`](/keys/campaignmoneydeltaeasy/) is the other difficulty's number.

```ini title="rulesmd.ini"
[General]
CampaignMoneyDeltaEasy=5000
CampaignMoneyDeltaHard=-5000
```
