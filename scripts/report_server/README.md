# Report receiver (draft)

**This is a draft. It is not deployed, and the game sends nothing until a player sets
`[Options] ReportURL`.** We need the server owner's permission before running it on their machine.

The game can offer a report after a crash, a freeze or an out-of-sync match (see
`code/report/`). It sends a zip of that game's files (the debug log, the SYNC files, the crash
dump) as an HTTPS POST, with a one-line JSON note in the `X-OpenYR-Report` header:

```json
{"format":1,"id":"20261024-010203-desync-4242-1","kind":"desync","commit":"6d60b4a9...","time":"2026-10-24T01:02:03Z","session":"1a2b3c4d seed 00c0ffee"}
```

These scripts receive and file the reports:

| File | Job |
| --- | --- |
| `receiver.py` | Takes `POST /report` on the loopback address. Caps the size (17 MB), limits each sender to 10 reports an hour, checks the zip (plain file names, 32 files, 64 MB unpacked, no damage), stores it and leaves a notice. |
| `notify.py` | Posts one line per notice to the staff channel, through the tester bot's own Discord client. |
| `test_receiver.py` | The checks, run without a network: `python3 -I -m unittest discover -s scripts/report_server`. |

## Where reports go

```
/var/lib/openyr-reports/
  crash/2026-10/<id>/report.zip, meta.json
  hang/2026-10/<id>/...
  desync/sessions/<session>/<id>/...     every seat's report from one match, together
  notify/<id>.json                       waiting for notify.py
```

An out-of-sync report names its match by the session identity and the random seed from the head
of its SYNC file, so every seat that sends one lands in the same `sessions/<session>` folder. The
folder name is a hash of the two.

`meta.json` keeps a short hash of the sender's address, for spotting floods, not the address
itself.

## Privacy

The logs hold player nicknames and details of the PC (Windows version, processor, graphics
card). The game's dialog says so before anything is sent. Keep the reports where only staff can
read them, and delete them once the bug is fixed. The staff channel line names no player.

## Deploying on the Debian server

Run these only once the server owner has agreed. `reports.example.org` stands for whatever name
we are given.

1. Make a user and the storage folder:

   ```sh
   sudo adduser --system --group --no-create-home openyr-reports
   sudo install -d -o openyr-reports -g openyr-reports -m 750 /var/lib/openyr-reports
   sudo install -d -o openyr-reports -g openyr-bot -m 770 /var/lib/openyr-reports/notify
   sudo adduser openyr-bot openyr-reports
   sudo install -d /opt/openyr-reports
   sudo cp receiver.py notify.py /opt/openyr-reports/
   ```

   The bot's user joins the reports group so that `notify.py`, which runs as that user, can
   reach the notify folder and remove each notice it has posted.

2. The receiver service, `/etc/systemd/system/openyr-reports.service`:

   ```ini
   [Unit]
   Description=OpenYR report receiver (draft)
   After=network-online.target

   [Service]
   User=openyr-reports
   Group=openyr-reports
   ExecStart=/usr/bin/python3 -I -u /opt/openyr-reports/receiver.py --root /var/lib/openyr-reports --port 8471
   Restart=always
   RestartSec=30
   NoNewPrivileges=yes
   ProtectSystem=strict
   ProtectHome=yes
   PrivateTmp=yes
   PrivateDevices=yes
   ReadWritePaths=/var/lib/openyr-reports

   [Install]
   WantedBy=multi-user.target
   ```

3. HTTPS in front of it. With nginx, in the server block for `reports.example.org`, which already
   has its certificate:

   ```nginx
   location = /report {
       client_max_body_size 17m;
       proxy_pass http://127.0.0.1:8471;
       proxy_set_header X-Forwarded-For $remote_addr;
   }
   ```

   Setting the header to `$remote_addr` alone, not appending to it, keeps a sender from naming
   its own address for the rate limit.

4. The notifier, as the bot's user so it can read the bot's token.
   `/etc/systemd/system/openyr-reports-notify.service`:

   ```ini
   [Unit]
   Description=Post new OpenYR reports to the staff channel (draft)

   [Service]
   Type=oneshot
   User=openyr-bot
   Group=openyr-bot
   EnvironmentFile=/etc/openyr-bot/env
   ExecStart=/usr/bin/python3 -I /opt/openyr-reports/notify.py --root /var/lib/openyr-reports --channel <staff channel id> --bot-scripts /opt/openyr-bot/scripts
   ```

   and `/etc/systemd/system/openyr-reports-notify.timer`:

   ```ini
   [Unit]
   Description=Post new OpenYR reports every minute (draft)

   [Timer]
   OnBootSec=1min
   OnUnitActiveSec=1min

   [Install]
   WantedBy=timers.target
   ```

5. Start them:

   ```sh
   sudo systemctl daemon-reload
   sudo systemctl enable --now openyr-reports.service openyr-reports-notify.timer
   ```

6. Point a test copy of the game at it with `[Options] ReportURL=https://reports.example.org/report`
   in its settings, and check that a forced crash arrives and is posted. Only then put the address
   in a tester build.
