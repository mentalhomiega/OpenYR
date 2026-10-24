"""Posts a line to the staff channel for each report receiver.py stored.

It reads the notices in <root>/notify, posts each through the tester bot's own Discord client
(scripts/discord.py in the bot's folder, which reads DISCORD_BOT_TOKEN), and removes a notice once
it is posted. Run it every minute from the systemd timer in README.md, as the bot's user.

  python3 -I notify.py --root /var/lib/openyr-reports --channel <staff channel id> --bot-scripts /opt/openyr-bot/scripts
"""

import argparse
import json
import pathlib
import sys


def size_text(size):
    if size < 1024 * 1024:
        return '%d KB' % ((size + 1023) // 1024)
    return '%.1f MB' % (size / (1024 * 1024))


def summary(notice):
    """The line posted for one report. It names no player: those are in the logs, for staff only."""
    kinds = {'crash': 'Crash', 'hang': 'Freeze', 'desync': 'Out-of-sync'}
    line = '%s report %s, build %s, %d files (%s), stored in `%s`' % (
        kinds.get(notice.get('kind'), 'Unknown'), notice.get('id', '?'), notice.get('commit', '?')[:12],
        len(notice.get('files', [])), size_text(int(notice.get('bytes', 0))), notice.get('folder', '?'))
    if notice.get('kind') == 'desync' and notice.get('session'):
        line += '. Seat %d from this match' % int(notice.get('seats', 1))
    return line


def pending(root):
    return sorted(pathlib.Path(root, 'notify').glob('*.json'))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--root', required=True)
    parser.add_argument('--channel', required=True, help='the staff channel id')
    parser.add_argument('--bot-scripts', required=True, help="the folder holding the bot's discord.py")
    args = parser.parse_args()

    sys.path.insert(0, args.bot_scripts)
    import discord  # noqa: E402  The bot's client, not the discord.py library.

    for path in pending(args.root):
        notice = json.loads(path.read_text(encoding='utf-8'))
        discord.call('POST', '/channels/%s/messages' % args.channel,
                     {'content': summary(notice), 'allowed_mentions': {'parse': []}})
        path.unlink()


if __name__ == '__main__':
    main()
