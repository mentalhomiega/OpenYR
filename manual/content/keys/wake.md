---
key: Wake
summary: The ripple laid on the water by something moving across it, falling into it or sinking through it.
see_also: [SplashList, IceBreakingWeight, Locomotor]
when_omitted:
  kind: value
  value: none
---

```ini title="rulesmd.ini"
[General]
Wake=MYWAKE ; an AnimType registered in [Animations]
```

The game reads the key from `[General]`. A value under `[AudioVisual]` is used only when `[General]` sets none.

The animation plays in three situations.

**Movement.** An object whose [`Locomotor`](/keys/locomotor/) is Drive, Hover or Levitate lays one at its position every tenth frame while it moves over open water and is not on a bridge.

**Splashes.** Some of the splashes that [`SplashList`](/keys/splashlist/) plays also lay a wake beside them. That page lists which ones.

**Ice giving way.** When ice breaks open into water, each object lost with it gets one. A vehicle that is not amphibious starts to sink, and an infantryman or aircraft is removed. An amphibious vehicle or a ship stays up and gets none. The exception is the vehicle whose weight broke the ice: it sinks even when it is amphibious, and then it gets no wake. [`IceBreakingWeight`](/keys/icebreakingweight/) and [`Fire`](/keys/fire/) cover what breaks ice.

With the key unset, nothing is laid in any of these situations.
