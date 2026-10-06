---
key: DebugOnly
scope: campaign
label: Test-only campaign
see_also: ["Description", "RequiredAddon"]
when_omitted:
  kind: value
  value: "no"
---

`yes` leaves the campaign out of the mission selection list. The shipped `BATTLEMD.INI` sets it on the test campaigns that start at a later mission, so the list offers only the Allied and Soviet campaigns.

The campaign is still read and keeps its place in the campaign numbering. A [client launch file](/formats/spawn-ini/#a-campaign-mission) whose `CampaignID` counts it still plays it.

```ini title="BATTLEMD.INI"
[Battles]
1=MYCAMP
2=MYTEST

[MYTEST]
Scenario=MYTEST03.MAP
Description=Test from mission 3
DebugOnly=yes
```
