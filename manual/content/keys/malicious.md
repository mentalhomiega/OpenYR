---
key: Malicious
summary: "With no, a hit by this warhead does not raise the harvester or base under attack alert."
see_also: [AllowZeroDamage, "system:base-attacked"]
when_omitted:
  kind: value
  value: "yes"
---

`Malicious` is an Ares key. With `Malicious=no`, damage from this warhead does not count as an attack for the alerts that tell the player their economy is under fire. A hit on a player's harvester vehicle then raises no harvester alert and plays no line. A hit on an undeployable gatherer structure, such as a deployed harvester, raises no alert for a player-controlled house either. A structure of any other kind still raises the usual base alert. A temporal warhead that starts to erase a harvester also raises no alert.

The warhead's damage is unchanged, and so is every other reaction to it. The key only keeps a harmless effect from alarming the player. A weapon with no warhead counts as malicious.

```ini title="rulesmd.ini"
[Prism]
Malicious=no
```
