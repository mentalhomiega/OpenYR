---
key: TogglePowerAllowed
summary: "With no, the Power Mode command and the sidebar power button do nothing."
see_also: [TogglePower, "system:power"]
when_omitted:
  kind: value
  value: "yes"
---

`TogglePowerAllowed` is an Ares key. With `no`, [Power Mode](/commands/togglepower/) and the sidebar's power button no longer enter power mode, so players cannot switch their structures off or on. The command still appears among the keyboard commands and can still be bound. Trigger actions and a scenario's structure records can still switch structures off.

Ares defaults the key to `no`, because Yuri's Revenge had no power mode. We default it to `yes`, so rules that leave it out keep the power mode our engine has always offered; set `TogglePowerAllowed=no` to match Ares.

```ini title="rulesmd.ini"
[General]
TogglePowerAllowed=no
```
