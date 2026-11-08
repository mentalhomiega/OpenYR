---
key: IonGreen
scope: scenarios
label: Scenario lighting
see_also: [Green, IonRed, IonBlue, "system:ion-storms"]
when_omitted:
  kind: value
  value: ".4"
---

While an ion storm runs, `IonGreen` replaces [`Green`](/keys/green/) as the multiplier on the green channel of every terrain palette. It also tints the [house color schemes](/glossary/#color-scheme) that are shaded by lighting, which `Green` never reaches. [`IonRed`](/keys/ionred/#scope-scenarios) covers how the ion tints replace the ordinary ones, and [the storm's lighting](/systems/ion-storms/#lighting) covers when they switch on and off.
