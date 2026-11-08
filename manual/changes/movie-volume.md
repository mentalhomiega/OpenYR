---
title: Add a volume slider for movie sound
category: feature
release: 0.2.0
targets:
- type: key
  id: MovieVolume
  effect: added
- type: key
  id: SoundVolume
  effect: changed
credit: [MentalHomiega]
---

Movie sound followed `SoundVolume`, and Bink movies ignored every volume setting and played at the engine's master volume. Movie sound now follows `MovieVolume`, which the sound options screen sets with a new Movies slider. A settings file without the key takes the `SoundVolume` level, so a Bink movie plays quieter than before until the slider is raised.
