---
key: TogglePowerAllowed
summary: "With yes, the player gets power mode: the Power Mode command and the modern sidebar's power button."
see_also: [TogglePower, "system:power"]
when_omitted:
  kind: value
  value: "no"
---

`TogglePowerAllowed` is an Ares key. With `yes`, [Power Mode](/commands/togglepower/) and the modern sidebar's power button enter power mode, in which players switch their structures off or on.

With `no`, the default as in Ares, there is no power mode, as in Yuri's Revenge: the command does nothing and is left out of the keyboard settings, and the modern sidebar has no power button, its other buttons closing up. The classic sidebar never has one. Trigger actions and a scenario's structure records can still switch structures off.

```ini title="rulesmd.ini"
[General]
TogglePowerAllowed=yes
```
