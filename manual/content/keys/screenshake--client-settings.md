---
key: ScreenShake
scope: client-settings
label: Screen shake
when_omitted:
  kind: value
  value: "yes"
---

`ScreenShake=yes` shakes the picture when a warhead or an ion blast detonates. `ScreenShake=no` draws the same picture without the shake. The map still records each shake, so the game state, saves and multiplayer checks are the same at either setting.

Other shakes do not move the picture at either setting. The routine that building sales and destroyed units call does nothing.

The **Screen shake** switch on the Game Controls page of the settings sets the key, and the change applies at once.
