---
key: ElectricAssault
summary: "Makes a soldier's second weapon charge allied Overpowerable structures."
see_also: ["system:power"]
when_omitted:
  kind: value
  value: "no"
---

A soldier whose second weapon has an `ElectricAssault=yes` warhead uses that weapon on an [`Overpowerable=yes`](/keys/overpowerable/) structure of its own or an allied house, and [charges](/systems/power/#overpowered-defenses) it instead of hurting it.

```ini title="rulesmd.ini"
[MyChargeBolt] ; example Warhead
ElectricAssault=yes
```
