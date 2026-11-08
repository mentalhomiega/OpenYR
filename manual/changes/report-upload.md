---
title: Offer crash, freeze and out-of-sync reports to the developers
category: feature
release: 0.2.0
targets:
- type: system
  id: out-of-sync-recovery
  effect: changed
credit:
- MentalHomiega
---

The game offers to send a report of a crash, a freeze or an out-of-sync match to the OpenYR developers. By default it asks first and sends to `https://94-237-94-104.sslip.io/report`.

At startup the game offers the folder of each crash or freeze from an earlier run that it has not asked about. After a match it offers the out-of-sync report this player's copy wrote, if any, and any freeze the game recovered from. The dialog names the kind of report, lists each file with its size, and gives the total size of the files and of the zip. It also names each file left out. **Send** queues the report and **Don't send** drops it. Either way the game does not ask about that folder again.

A report holds these files:

- For a crash or freeze: the minidump, `minidump.dmp`, and the `.txt` and `.log` files of the folder in `Exceptions` beside the game, such as the crash summary and the end of the debug log.
- For an out-of-sync match: this player's `SYNC_H*.LOG` and the debug log of the session.

A full memory dump, such as `fulldump.dmp`, is never included.

A report is at most 4 MB as zipped. A file that would take the zip past 4 MB is left out and named in the dialog. The files after it are still added.

**The logs contain player nicknames and details of this PC**, such as its Windows version, processor and graphics card. The dialog says so before anything is sent.

A queued report waits in `Reports\pending` in the user folder. The game sends it in the background at startup and after each match. The report is an HTTPS POST of the zip, with a note that holds the build's commit, the kind of report and the time. An out-of-sync report's note also names its match by the session identity and the random seed, so the receiver can file every player's report from the same match together.

The receiver's answer decides what happens to a waiting report:

- 2xx: the receiver has the report, and it leaves the queue.
- 4xx: the report leaves the queue and is not retried.
- No answer, such as a network error, or 5xx: the report stays queued for the next game start. After three game starts that each failed to send it, the report is dropped. A game start counts once, however many times the game tries to send in that run.

The game reads these keys in the `[Options]` section of the settings file:

| Key | Default | Meaning |
| --- | --- | --- |
| `ReportUpload` | `ask` | `ask` shows the dialog for each report, `always` queues each one without asking, and `never` sends nothing. |
| `ReportURL` | `https://94-237-94-104.sslip.io/report` | The receiver's address. An empty value turns reporting off, whatever `ReportUpload` says. |
