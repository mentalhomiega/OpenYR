---
key: ConcentricRadialIndicator
scope: buildingtype
label: Pulsing radius ring
summary: Whether the structure's radius ring blinks and pulses instead of staying steady.
see_also: [HasRadialIndicator, PsychicDetectionRadius]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rulesmd.ini"
[NAPSIS] ; Psychic Sensor
HasRadialIndicator=true
ConcentricRadialIndicator=true
```

With `yes`, the [radius ring](/keys/hasradialindicator/) of a selected structure blinks. It shows for about one second in every two, fading from white to the owner's color as it appears. A second ring grows from the centre to the full radius over the same two seconds. The second ring appears only once its horizontal radius is over 32 pixels.

With `no`, the ring is steady. The key has no effect without `HasRadialIndicator=yes`.
