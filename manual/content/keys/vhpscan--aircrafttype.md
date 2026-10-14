---
key: VHPScan
scope: aircrafttype
label: Weigh targets by estimated health
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "None"
---

Sets how this type's target scans treat a candidate's estimated health. The estimate is the target's health less the damage that shots already fired at it are expected to deal, so it shows how close the target is to dying before the shots land.

| Value | Effect |
| --- | --- |
| `None` | The scan ignores the estimate. |
| `Normal` | A candidate at half its strength or less scores double. A candidate whose estimate is below `1` scores half. |
| `Strong` | The scan passes over a candidate whose estimate is below `1`. |

The value is matched without regard to case. Any other value leaves the setting unchanged.

For example, `VHPScan=Strong` on a group of tanks stops them all turning onto a target that the shots already in flight will destroy, and sends the spare guns to the next target instead.
