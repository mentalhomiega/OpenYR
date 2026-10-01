---
key: FireInTransport
summary: "Lets a passenger fire this weapon from inside an open-topped transport."
see_also: [OpenTopped, OpenToppedAnim, "system:transports"]
when_omitted:
  kind: value
  value: "yes"
---

A passenger riding an [open-topped transport](/systems/transports/#firing-from-an-open-topped-transport) cannot fire a weapon set to `no`.

```ini title="rulesmd.ini"
[MyCharge] ; example Weapon
FireInTransport=no
```
