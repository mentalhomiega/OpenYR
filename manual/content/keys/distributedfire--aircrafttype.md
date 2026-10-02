---
key: DistributedFire
scope: aircrafttype
label: 'Spreads fire across targets'
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the object spreads its shots across the targets around it instead of firing at one until it dies. The Aegis cruiser uses this against groups of aircraft.

```ini title="rulesmd.ini"
[MYCRUISER] ; example VehicleType
DistributedFire=yes
```

Each time the object looks for a target on its own, it keeps every target its search accepts, with the threat value the search gave it. After each shot it drops its target and notes the one it shot. It then picks the most threatening target it has not shot at since it last went through them all, and once it has shot at every one it starts over with the most threatening. A target that leaves the search is forgotten.

A vehicle, infantryman or aircraft on an attack mission, such as one the player ordered to attack a target, keeps firing at that target. A structure always spreads its fire.
