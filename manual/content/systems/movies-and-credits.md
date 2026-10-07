---
title: Movies and credits
summary: The Movies & Credits menu plays the Renegade sneak peek, any campaign movie from a list, and the credits roll from CREDITSMD.TXT.
category: interface-controls
keys: [StretchMovies, NetID]
related:
  - type: format
    id: bink
  - type: format
    id: vqa
  - type: key
    id: MenuStyle
    scope: client-settings
---

Movies & Credits on the main menu opens three choices: Sneak Peeks, Play Movies and View Credits. Back, or Escape, returns to the main menu. The menu is the same in both [menu styles](/keys/menustyle/); the classic style draws the choices in its right-hand column.

## Sneak Peeks

Sneak Peeks plays `RENEGADE.BIK`, the movie Yuri's Revenge shows for this choice. When the game files have no `RENEGADE.BIK`, it plays the Tiberian Sun intro movie of the player's side and then `SIZZLE1.VQA`. The main menu music stops for the movie and starts again afterwards. The menu then returns to the main menu, not to Movies & Credits.

## Play Movies

Play Movies opens a list of the campaign movies the player has seen. The intro movie is always first. Then come the Soviet movies, up to the latest one seen: the seven mission movies and the Soviet victory movie, in the order they are played. The Allied movies follow in the same way. Each row shows the movie's name from the string table, such as "Operation: Time Shift". A movie whose `.BIK` file the game cannot find is left out of the list.

Select a row and choose Play Movie, or double-click the row. The movie plays full screen, and when it ends or is skipped the list opens again, with the same row selected. Back, or Escape, closes the list.

A movie counts as seen when the game is about to play it, and playing one opens the earlier movies of its campaign too: after the third Soviet movie, the first three are listed. The movies seen are kept in [`NetID=`](/keys/netid/), which Yuri's Revenge writes in the same form. A game with no movies seen lists only the intro movie.

## View Credits

View Credits scrolls the text of `CREDITSMD.TXT` up the screen, with the `CREDITS` music track queued. If the game files have no `CREDITSMD.TXT`, it shows `TSCREDIT.TXT` with the `MADRAP` track, as Tiberian Sun does. Escape ends the roll. The menu then returns to the main menu.

`CREDITSMD.TXT` is read as text. A file that is not valid UTF-8 is read as Latin-1. A `{label}` in a line is replaced by that label's text from the string table; a label the table lacks is shown as its own name. Each line is placed by its indent:

| Indent | Placement |
| --- | --- |
| Columns 4 to 7 | Centered |
| Fewer than 3 columns | Right-justified, left of the center |
| Any other column | Left-justified from the center |

A centered line that follows at least one blank line is drawn in a lighter shadow, which sets headings apart from the lines under them.

The [`StretchMovies`](/keys/stretchmovies/) setting scales the movies in this menu as it scales any other full-screen movie.
