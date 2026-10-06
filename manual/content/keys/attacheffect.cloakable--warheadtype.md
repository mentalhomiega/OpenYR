---
key: AttachEffect.Cloakable
scope: warheadtype
label: Cloak while attached
when_omitted:
  kind: value
  value: "no"
---

`AttachEffect.Cloakable=yes` lets each object carrying this warhead's effect cloak while the effect lasts, under the same conditions as a `Cloakable=yes` type. When the effect ends, an object that cannot otherwise cloak uncloaks, unless it stands in a cloaking field.
