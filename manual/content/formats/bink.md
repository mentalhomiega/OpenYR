---
format_id: bink
title: Bink video
summary: Stores the full-motion video Yuri's Revenge ships, which the engine plays full screen through the BINKW32.DLL of the player's own install.
kind: binary
extensions:
  - .BIK
role: video
related:
  - { type: format, id: vqa }
  - { type: format, id: mix }
  - { type: system, id: movies-and-credits }
source_files:
  - code/binkmovie.cpp
  - code/movie.cpp
---

A `.BIK` file holds a Bink movie. Yuri's Revenge ships its movies in this format inside its movie archives, such as `MOVMD03.MIX`. OpenTS plays a Bink movie only through `BINKW32.DLL`, which belongs to the player's Yuri's Revenge install and is not part of OpenTS. The game looks for it in the game's data folder first and then on the normal DLL search path.

When the DLL is missing, or is not the 32-bit Bink 1 library Yuri's Revenge uses, a Bink movie does not play and the game goes on. This is also the case in a build that is not a 32-bit Windows build.

## Which movies play as Bink

A request to play a movie named with `.BIK` plays that file. A request for a `.VQA` file that does not exist plays the `.BIK` file of the same name when that exists, so a scenario's [`Intro`](/keys/intro/) or `Win` movie can be shipped as either format. A `.VQA` file that exists always plays as a [VQA movie](/formats/vqa/).

Only full-screen movies play as Bink. The radar pane and the graphical menus play VQA movies only.

## Playback

The game reads the whole movie file into memory, then draws it a frame at a time at the movie's own size, centered on the screen. A movie larger than the screen is cropped to its middle: an 800 by 600 movie on a 640 by 480 screen loses 80 pixels at each side and 60 at the top and bottom. [`StretchMovies`](/keys/stretchmovies/) scales the whole movie to fit instead, as it does for a VQA movie.

- Escape ends the movie. In a game against other machines, the votes described in [Multiplayer movies](/systems/multiplayer-movies/) decide.
- The movie pauses while the game window is in the background and continues when it returns.
- The sound plays through Bink's own DirectSound output, at the engine's master volume. The movie, music and sound volume settings do not change it. While the master volume is zero, as in a scripted test run, the movie plays without opening a sound device.
