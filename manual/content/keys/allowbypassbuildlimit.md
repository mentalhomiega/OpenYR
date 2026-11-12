---
key: AllowBypassBuildLimit
summary: Per difficulty, whether a computer house may create a team that takes a type past its BuildLimit.
see_also: [BuildLimit, AllowParallelAIQueues, TeamDelays]
when_omitted:
  kind: value
  value: "yes for every difficulty"
---

`AllowBypassBuildLimit` is an Ares key, read from `[GlobalControls]`. It holds three booleans, one for each difficulty in the order of the other difficulty lists such as [`TeamDelays`](/keys/teamdelays/). Where an entry is `no`, a computer house at that difficulty does not create a team whose task force asks for more of a type than its positive [`BuildLimit`](/keys/buildlimit/) still allows, counting the objects of that type the house owns. A missing entry reads as `no`. Types with a limit of zero or less are not checked.

Without the key every difficulty creates its teams as the original game does, whatever the build limits. The Ares manual gives `no,no,no` as its default and calls the entries easy, medium and hard; which entry Ares applies to which computer was not observed.

```ini title="rulesmd.ini"
[GlobalControls]
AllowBypassBuildLimit=yes,yes,no
```
