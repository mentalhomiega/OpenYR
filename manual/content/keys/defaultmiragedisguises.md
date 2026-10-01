---
key: DefaultMirageDisguises
summary: "The terrain types a still DisguiseWhenStill vehicle picks its look from."
see_also: [DisguiseWhenStill, "system:disguises"]
when_omitted:
  kind: value
  value: none
---

A [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle that stops picks one of these terrain types at random to [look like](/systems/disguises/#vehicles-that-hide-as-terrain). With none listed, it never disguises.

```ini title="rulesmd.ini"
[General]
DefaultMirageDisguises=TREE01,TREE02,TREE03,TREE04
```
