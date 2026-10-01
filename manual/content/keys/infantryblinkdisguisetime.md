---
key: InfantryBlinkDisguiseTime
summary: "How long an enemy soldier beside a DisguiseWhenStill vehicle keeps it from disguising, in frames."
see_also: [DisguiseWhenStill, "system:disguises"]
when_omitted:
  kind: value
  value: "0"
---

After a soldier of a house that is not an ally stands next to a [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle, the vehicle cannot [take a new disguise](/systems/disguises/#vehicles-that-hide-as-terrain) for this many frames.

```ini title="rulesmd.ini"
[General]
InfantryBlinkDisguiseTime=20
```
