---
key: AuxSound1
summary: The sound a vehicle makes as it deploys, a structure as it turns back into its vehicle, and a flying object as it takes off.
see_also: [AuxSound2, DeploySound, Locomotor]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[MYHELI] ; an AircraftType registered in [AircraftTypes]
AuxSound1=MYHELI_Takeoff ; a sound ID registered in SOUND.INI
```

These events play this sound at the object's position, whichever house owns it:

- A **vehicle** that [`DeploysInto`](/keys/deploysinto/) a structure plays it as it unpacks into the structure. An [`IsSimpleDeployer=yes`](/keys/issimpledeployer/) vehicle plays it as it starts to deploy.
- A **structure** with [`UndeploysInto`](/keys/undeploysinto/) plays it as it turns back into that vehicle.
- An object moved by the flyer [locomotor](/keys/locomotor/#scope-aircrafttype) plays it each time it takes off, unless it is stunned.

On infantry types the key has no effect.

:::caution[`DeploySound` wins]
[`DeploySound`](/keys/deploysound/#scope-buildingtype) sets the same sound on a BuildingType or UnitType and is read after `AuxSound1`. A section that sets both plays only the `DeploySound`. A rules file read later that sets only `AuxSound1` replaces it.
:::

A name that matches no sound ID is ignored and the sound set earlier stays. Writing `none` therefore cannot clear a sound that an earlier rules file set.
