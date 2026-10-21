---
key: ScriptedCameraZoom
summary: Zooms the map to the scene height while a scripted scene takes the input or moves the camera.
see_also: [ScriptedCameraHeight, ZoomInFactor]
when_omitted:
  kind: value
  value: "yes"
---

`ScriptedCameraZoom=yes` zooms the map view to the height that [`ScriptedCameraHeight`](/keys/scriptedcameraheight/) sets when a scripted scene takes the input away or moves the camera. When control returns, the player's zoom comes back. `ScriptedCameraZoom=no` leaves the zoom where the player set it. The [Map zoom](/systems/map-zoom/#scripted-scenes) page lists the actions that apply the scene zoom and how the player's zoom returns.

The **Cinematic zoom for scripted scenes** switch on the Accessibility page of the settings sets the key. OK applies the change and writes it to `RA2MD.INI`. Back discards it.
