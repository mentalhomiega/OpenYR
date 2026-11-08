---
key: IonBlue
scope: scenarios
label: Scenario lighting
see_also: [Blue, IonRed, IonGreen, "system:ion-storms"]
when_omitted:
  kind: value
  value: ".75"
---

While an ion storm runs, `IonBlue` replaces [`Blue`](/keys/blue/) as the multiplier on the blue channel of every terrain palette. It also tints the [house color schemes](/glossary/#color-scheme) that are shaded by lighting, which `Blue` never reaches. [`IonRed`](/keys/ionred/#scope-scenarios) covers how the ion tints replace the ordinary ones, and [the storm's lighting](/systems/ion-storms/#lighting) covers when they switch on and off.
