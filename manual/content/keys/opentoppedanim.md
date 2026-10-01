---
key: OpenToppedAnim
summary: "The firing animation this weapon plays when a passenger fires it from an open-topped transport."
see_also: [Anim, FireInTransport, "system:transports"]
when_omitted:
  kind: value
  value: none
---

Plays at the firing position when a passenger fires this weapon from an [open-topped transport](/systems/transports/#firing-from-an-open-topped-transport) and the weapon has no [`Anim`](/keys/anim/).

```ini title="rulesmd.ini"
[MyRifle] ; example Weapon
OpenToppedAnim=MYFLASH ; an AnimType registered in [Animations]
```
