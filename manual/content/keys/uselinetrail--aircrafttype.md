---
key: UseLineTrail
scope: aircrafttype
label: Fading line trail
see_also: [LineTrailColor, LineTrailColorDecrement, LineTrailColorOverride, Trailer, "system:projectile-flight"]
when_omitted:
  kind: value
  value: "no"
---

`UseLineTrail=yes` in an object type's art section gives each object of the type a line that follows it. The line joins the object's last 32 positions, newest first, so it shows where the object has been. It is drawn in [`LineTrailColor`](/keys/linetrailcolor/) and fades by [`LineTrailColorDecrement`](/keys/linetrailcolordecrement/) of its 255 opacity every frame. The line starts when the object is placed on the map.

```ini title="art.ini"
[MYMISSILE] ; the art section of a projectile, vehicle or other object type
UseLineTrail=yes
LineTrailColor=255,40,40
LineTrailColorDecrement=8 ; the line is gone 32 frames after the object stops
```

A new position is added only on a frame where the object has moved, so an object that stands still leaves no new line and the old line fades out. A line is hidden where something nearer stands. When the object is destroyed or leaves the map, its line fades out where it was. The line is not saved, so a game that is loaded shows none until the objects are placed again. With the game's detail level on Low, every line fades twice as fast.

The key is read from the art section named by the type's [`Image`](/keys/image/), and applies to every kind of object type that has one.
