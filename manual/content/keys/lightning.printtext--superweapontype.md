---
key: Lightning.PrintText
scope: superweapontype
summary: "Chooses whether a lightning storm warns the players."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningPrintText"
---

With `yes`, a `Type=LightningStorm` weapon plays the EVA warning and prints the approach message while its storm waits to break, and plays [`StormSound`](/keys/stormsound/) and prints a message when it breaks. With `no`, the storm gives no warning.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.PrintText=no
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
