---
title: Offer crash, freeze and out-of-sync reports to the developers (draft)
category: feature
release: 0.2.0
targets:
- type: system
  id: out-of-sync-recovery
  effect: changed
credit:
- MentalHomiega
---

**Not active.** This draft sends nothing: no receiver exists yet, and with `ReportURL` empty, the default, the game never shows the dialog or sends a report.

Once `ReportURL` names a receiver, the game can offer a report of a problem to the developers. At startup it offers the folder of each crash or freeze from an earlier run that it has not asked about. After a match it offers the out-of-sync report this player's copy wrote, if any, and any freeze the game recovered from. The dialog names the kind of report and lists each file with its size and the total. **Send** queues the report and **Don't send** drops it. Either way the game does not ask about that folder again.

A crash or freeze report holds the files of its folder in `Exceptions` beside the game: the crash or freeze summary, the minidump and the end of the debug log. An out-of-sync report holds this player's `SYNC_H*.LOG` and the debug log of the session. A report is at most 16 MB, and a file that would take it past that is left out and named as left out in the dialog.

**The logs contain player nicknames and details of this PC**, such as its Windows version, processor and graphics card. The dialog says so before anything is sent.

A queued report waits in `Reports\pending` in the user folder and goes out in the background, so the game does not wait for it. A report that fails to send stays there and goes at the next start. It travels as an HTTPS POST of a zip, with a note of the build's commit, the kind of report and the time. An out-of-sync report's note also names its match by the session identity and the random seed, so the receiver can file every player's report from the same match together.

The game reads these keys in the `[Options]` section of the settings file:

| Key | Default | Meaning |
| --- | --- | --- |
| `ReportUpload` | `ask` | `ask` shows the dialog for each report, `always` queues each one without asking, and `never` sends nothing. |
| `ReportURL` | empty | The receiver's address. Empty means no dialog and nothing sent, whatever `ReportUpload` says. |
