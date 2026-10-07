---
key: NetID
scope: client-settings
label: Movies seen
see_also: [MenuStyle]
when_omitted:
  kind: value
  value: "0 0"
  note: No campaign movie counts as seen, so Play Movies lists only the intro movie.
---

`NetID=` in `[Network]` records how many campaign movies the player has seen. The game writes it to `RA2MD.INI`, in the same form as Yuri's Revenge, so a settings file from either game carries the movies seen over to the other.

The entry holds two numbers separated by a space, the Soviet count and then the Allied count, each from 0 to 8. A count of 3 means the first three movies of that campaign have been seen. The text is stored as a list of the 16-bit characters of those numbers in hexadecimal, each with all its bits flipped and followed by a comma, so `3 2` is written as `ffcc,ffdf,ffcd,`.

```ini title="RA2MD.INI"
[Network]
NetID=ffcc,ffdf,ffcd,
```

A movie counts as seen when a campaign movie is about to play, whether in a mission or from the Play Movies list, and the file is saved at once. A movie opens itself and every earlier movie of its campaign, so seeing the third Soviet movie raises the Soviet count to 3 and playing an earlier one changes nothing. The intro movie is always listed and does not change the entry. The [Movies and credits](/systems/movies-and-credits/) page has the list and its order.

A count outside 0 to 8 is read as 0, and so is a missing one, as is an entry that is not a list of hexadecimal numbers.
