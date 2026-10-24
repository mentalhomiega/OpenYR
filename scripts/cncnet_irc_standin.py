"""A small IRC server on 127.0.0.1 that stands in for the CnCNet chat network.

It speaks the part of IRC the CnCNet lobby uses: registration, PING, JOIN and PART with channel
keys and limits, NAMES, TOPIC, PRIVMSG and NOTICE (CTCP included), MODE +k/+l/+i/+o, KICK and
QUIT. Optional bot players fill the lobby: they chat in the main channel and announce games in the
broadcast channel the way a CnCNet host does, so the lobby screen has something to show.

With --openyr-build and --room-password, two bots also speak the OpenYR room protocol: the first
hosts a password-protected OpenYR room that answers a player's hello, and the last joins any game a
player announces, says hello and gets ready. The build must be the game's build id, which its debug
log prints as "[CnCNet] Room build id".

  python scripts/cncnet_irc_standin.py [--port 6667] [--bots 3] [--game-version 1.001]
      [--log FILE] [--openyr-build TEXT --room-password TEXT]

Nothing here leaves the machine: the server binds to the loopback address only.
"""

import argparse
import asyncio
import hashlib
import random
import time

SERVER = 'standin.cncnet.test'
CHAT = '#cncnet-yr'
BROADCAST = '#cncnet-yr-games'
MAPS = [('Little Big Lake', 4), ('Heck Freezes Over', 4), ('Golden State Fwy', 6), ('Lake Blitz', 4),
        ('Tour of Egypt', 4), ('Dry Heat', 2), ('Isle of War', 4), ('Sinkhole', 8)]


def fold(name):
    return name.lower().replace('[', '{').replace(']', '}').replace('\\', '|').replace('~', '^')


def parse(line):
    prefix = ''
    if line.startswith('@'):
        line = line.split(' ', 1)[1] if ' ' in line else ''
    if line.startswith(':'):
        prefix, _, line = line[1:].partition(' ')
    if ' :' in line:
        head, _, trailing = line.partition(' :')
        parts = head.split()
        parts.append(trailing)
    else:
        parts = line.split()
    if not parts:
        return None
    return prefix, parts[0].upper(), parts[1:]


class Channel:
    def __init__(self, name):
        self.name = name
        self.members = {}       # folded nick -> client
        self.ops = set()
        self.topic = ''
        self.key = ''
        self.limit = 0
        self.invite_only = False


class Client:
    def __init__(self, server, writer):
        self.server = server
        self.writer = writer
        self.nick = ''
        self.ident = ''
        self.realname = ''
        self.registered = False
        self.channels = set()
        self.bot = None

    @property
    def mask(self):
        return '%s!%s@%s' % (self.nick, self.ident or 'user', 'standin.local')

    def send(self, line):
        if self.bot is not None:
            self.bot.receive(line)
            return
        self.server.log('-> %s %s' % (self.nick or '?', line))
        try:
            self.writer.write((line + '\r\n').encode('utf-8', 'replace'))
        except Exception:
            pass

    def numeric(self, code, *params):
        text = ' '.join(params[:-1] + (':' + params[-1],)) if params else ''
        self.send(':%s %s %s %s' % (SERVER, code, self.nick or '*', text))


class Server:
    def __init__(self, log_path):
        self.clients = {}       # folded nick -> client
        self.channels = {}
        self.bots = []
        self.log_file = open(log_path, 'a', encoding='utf-8') if log_path else None

    def log(self, text):
        if self.log_file:
            self.log_file.write('%s %s\n' % (time.strftime('%H:%M:%S'), text))
            self.log_file.flush()

    def channel_send(self, channel, line, skip=None):
        for member in list(channel.members.values()):
            if member is not skip:
                member.send(line)

    def handle(self, client, line):
        self.log('<- %s %s' % (client.nick or '?', line))
        message = parse(line)
        if not message:
            return
        _, command, params = message
        handler = getattr(self, 'cmd_' + command.lower(), None)
        if handler is None:
            if client.registered:
                client.numeric('421', command, 'Unknown command')
            return
        if not client.registered and command not in ('NICK', 'USER', 'PING', 'PONG', 'QUIT'):
            client.numeric('451', 'You have not registered')
            return
        handler(client, params)

    def try_register(self, client):
        if client.registered or not client.nick or not client.ident:
            return
        client.registered = True
        self.clients[fold(client.nick)] = client
        client.numeric('001', 'Welcome to the CnCNet stand-in %s' % client.mask)
        client.numeric('002', 'Your host is %s' % SERVER)
        client.numeric('376', 'End of /MOTD command.')

    def cmd_nick(self, client, params):
        if not params:
            client.numeric('431', 'No nickname given')
            return
        nick = params[0][:15]
        valid = nick and not nick[0].isdigit() and all(c.isalnum() or c in '-_[]\\`^{}|' for c in nick)
        if not valid:
            client.numeric('432', nick, 'Erroneous nickname')
            return
        owner = self.clients.get(fold(nick))
        if owner is not None and owner is not client:
            client.numeric('433', nick, 'Nickname is already in use')
            return
        if client.registered:
            old_mask = client.mask
            del self.clients[fold(client.nick)]
            self.clients[fold(nick)] = client
            seen = {client}
            client.send(':%s NICK :%s' % (old_mask, nick))
            for name in client.channels:
                channel = self.channels[name]
                channel.members.pop(fold(client.nick), None)
                channel.members[fold(nick)] = client
                if fold(client.nick) in channel.ops:
                    channel.ops.discard(fold(client.nick))
                    channel.ops.add(fold(nick))
                for member in channel.members.values():
                    if member not in seen:
                        seen.add(member)
                        member.send(':%s NICK :%s' % (old_mask, nick))
        client.nick = nick
        self.try_register(client)

    def cmd_user(self, client, params):
        if len(params) < 4:
            client.numeric('461', 'USER', 'Not enough parameters')
            return
        client.ident = params[0][:10]
        client.realname = params[3]
        self.try_register(client)

    def cmd_ping(self, client, params):
        client.send(':%s PONG %s :%s' % (SERVER, SERVER, params[0] if params else ''))

    def cmd_pong(self, client, params):
        pass

    def cmd_join(self, client, params):
        if not params:
            return
        keys = params[1].split(',') if len(params) > 1 else []
        for index, name in enumerate(params[0].split(',')):
            if not name.startswith('#'):
                client.numeric('403', name, 'No such channel')
                continue
            folded = fold(name)
            channel = self.channels.get(folded)
            if channel is None:
                channel = self.channels[folded] = Channel(name)
                channel.ops.add(fold(client.nick))
            elif fold(client.nick) in channel.members:
                continue
            else:
                key = keys[index] if index < len(keys) else ''
                if channel.key and key != channel.key:
                    client.numeric('475', channel.name, 'Cannot join channel (+k)')
                    continue
                if channel.limit and len(channel.members) >= channel.limit:
                    client.numeric('471', channel.name, 'Cannot join channel (+l)')
                    continue
                if channel.invite_only:
                    client.numeric('473', channel.name, 'Cannot join channel (+i)')
                    continue
            channel.members[fold(client.nick)] = client
            client.channels.add(folded)
            self.channel_send(channel, ':%s JOIN %s' % (client.mask, channel.name))
            if channel.topic:
                client.numeric('332', channel.name, channel.topic)
            self.send_names(client, channel)
            for bot in self.bots:
                if bot.client is not client:
                    bot.joined(channel, client)

    def send_names(self, client, channel):
        names = [('@' if nick in channel.ops else '') + member.nick for nick, member in channel.members.items()]
        for start in range(0, len(names), 40):
            client.numeric('353', '=', channel.name, ' '.join(names[start:start + 40]))
        client.numeric('366', channel.name, 'End of /NAMES list.')

    def cmd_names(self, client, params):
        channel = self.channels.get(fold(params[0])) if params else None
        if channel:
            self.send_names(client, channel)

    def leave(self, client, channel):
        channel.members.pop(fold(client.nick), None)
        channel.ops.discard(fold(client.nick))
        client.channels.discard(fold(channel.name))
        if not channel.members:
            del self.channels[fold(channel.name)]

    def cmd_part(self, client, params):
        for name in (params[0].split(',') if params else []):
            channel = self.channels.get(fold(name))
            if channel and fold(client.nick) in channel.members:
                self.channel_send(channel, ':%s PART %s' % (client.mask, channel.name))
                self.leave(client, channel)

    def cmd_topic(self, client, params):
        channel = self.channels.get(fold(params[0])) if params else None
        if channel is None:
            return
        if len(params) < 2:
            client.numeric('332', channel.name, channel.topic)
            return
        if fold(client.nick) not in channel.ops:
            client.numeric('482', channel.name, "You're not channel operator")
            return
        channel.topic = params[1]
        self.channel_send(channel, ':%s TOPIC %s :%s' % (client.mask, channel.name, channel.topic))

    def cmd_mode(self, client, params):
        if not params:
            return
        channel = self.channels.get(fold(params[0]))
        if channel is None:
            return
        if len(params) == 1:
            return
        if fold(client.nick) not in channel.ops:
            client.numeric('482', channel.name, "You're not channel operator")
            return
        modes, args, adding = params[1], list(params[2:]), True
        applied = []
        for mode in modes:
            if mode in '+-':
                adding = mode == '+'
            elif mode == 'k':
                channel.key = args.pop(0) if adding and args else ''
                applied.append(('+k ' + channel.key) if adding else '-k')
            elif mode == 'l':
                channel.limit = int(args.pop(0)) if adding and args else 0
                applied.append(('+l %d' % channel.limit) if adding else '-l')
            elif mode == 'i':
                channel.invite_only = adding
                applied.append('+i' if adding else '-i')
            elif mode == 'o' and args:
                target = fold(args.pop(0))
                if target in channel.members:
                    (channel.ops.add if adding else channel.ops.discard)(target)
                    applied.append(('+o ' if adding else '-o ') + channel.members[target].nick)
        for change in applied:
            self.channel_send(channel, ':%s MODE %s %s' % (client.mask, channel.name, change))

    def cmd_kick(self, client, params):
        if len(params) < 2:
            return
        channel = self.channels.get(fold(params[0]))
        if channel is None or fold(client.nick) not in channel.ops:
            return
        victim = channel.members.get(fold(params[1]))
        if victim is None:
            return
        reason = params[2] if len(params) > 2 else client.nick
        self.channel_send(channel, ':%s KICK %s %s :%s' % (client.mask, channel.name, victim.nick, reason))
        self.leave(victim, channel)

    def relay(self, client, params, command):
        if len(params) < 2:
            return
        for target in params[0].split(','):
            line = ':%s %s %s :%s' % (client.mask, command, target, params[1])
            if target.startswith('#'):
                channel = self.channels.get(fold(target))
                if channel is None:
                    client.numeric('401', target, 'No such nick/channel')
                elif fold(client.nick) not in channel.members:
                    client.numeric('404', target, 'Cannot send to channel')
                else:
                    self.channel_send(channel, line, skip=client)
            else:
                other = self.clients.get(fold(target))
                if other is None:
                    client.numeric('401', target, 'No such nick/channel')
                else:
                    other.send(line)

    def cmd_privmsg(self, client, params):
        self.relay(client, params, 'PRIVMSG')

    def cmd_notice(self, client, params):
        self.relay(client, params, 'NOTICE')

    def cmd_whois(self, client, params):
        other = self.clients.get(fold(params[-1])) if params else None
        if other:
            client.numeric('311', other.nick, other.ident, 'standin.local', '*', other.realname)
        client.numeric('318', params[-1] if params else '*', 'End of /WHOIS list.')

    def cmd_quit(self, client, params):
        self.drop(client, params[0] if params else 'Quit')

    def drop(self, client, reason):
        if client.registered:
            seen = set()
            for name in list(client.channels):
                channel = self.channels.get(name)
                if channel is None:
                    continue
                for member in channel.members.values():
                    if member is not client and member not in seen:
                        seen.add(member)
                        member.send(':%s QUIT :%s' % (client.mask, reason))
                self.leave(client, channel)
            self.clients.pop(fold(client.nick), None)
            client.registered = False
        if client.bot is None:
            client.send('ERROR :Closing Link: (%s)' % reason)
            try:
                client.writer.close()
            except Exception:
                pass


class Bot:
    """A scripted player: joins the chat and broadcast channels, talks now and then, and hosts a
    game whose listing it re-announces every 30 seconds, as CnCNet hosts do."""

    LINES = ['anyone up for a 2v2?', 'gg', 'need one more for Lake Blitz', 'brb', 'who wants a rematch',
             'hosting now, join up']

    def __init__(self, server, nick, game_version, hosts, greets=False):
        self.server = server
        self.greets = greets
        self.client = Client(server, None)
        self.client.bot = self
        self.game_version = game_version
        self.hosts = hosts
        server.cmd_nick(self.client, [nick])
        server.cmd_user(self.client, ['yr.' + hashlib.sha1(nick.encode()).hexdigest()[:6], '0', '*', '9.3.5 YR CnCNet'])
        server.cmd_join(self.client, [CHAT])
        server.cmd_join(self.client, [BROADCAST])
        number = random.randint(1000000, 9999999)
        self.room = '%s-game%d' % (CHAT, number)
        self.map_name, self.max_players = random.choice(MAPS)
        self.players = [nick]

    def receive(self, line):
        message = parse(line)
        if message and message[1] == 'PRIVMSG' and fold(message[2][0]) == fold(self.client.nick):
            sender = message[0].split('!')[0]
            self.server.cmd_privmsg(self.client, [sender, 'hi %s, I am a stand-in bot' % sender])

    def joined(self, channel, client):
        # The first bot greets a player joining the chat channel, so a test sees a chat line soon.
        if self.greets and fold(channel.name) == fold(CHAT) and client.bot is None:
            self.server.cmd_privmsg(self.client, [CHAT, 'welcome %s' % client.nick])

    def listing(self, closed=False):
        flags = '0%s%s00' % ('0', '1' if closed else '0')
        fields = ['A', self.game_version, str(self.max_players), self.room, "%s's game" % self.client.nick,
                  flags, ','.join(self.players), self.map_name, 'Standard', '[DYN]', '0', '0',
                  hashlib.sha1(self.map_name.encode()).hexdigest(), '']
        return ';'.join(fields)

    async def run(self):
        await asyncio.sleep(random.uniform(1, 10))
        while self.client.registered:
            if self.hosts:
                self.server.cmd_notice(self.client, [BROADCAST, '\x01GAME %s\x01' % self.listing()])
            if random.random() < 0.3:
                self.server.cmd_privmsg(self.client, [CHAT, random.choice(self.LINES)])
            await asyncio.sleep(30)


def ctcp(text):
    if len(text) >= 2 and text[0] == '\x01' and text[-1] == '\x01':
        command, _, arguments = text[1:-1].partition(' ')
        return command, arguments
    return None, None


def escape(text):
    return ''.join('%%%02X' % ord(c) if c in ';,=|%' or ord(c) < 0x20 else c for c in text)


class OpenYRHostBot(Bot):
    """Hosts an OpenYR room behind the password: seats a player whose hello names the same build,
    accepts every setup request and marks ready players."""

    def __init__(self, server, nick, game_version, build, password):
        super().__init__(server, nick, game_version, hosts=True)
        self.build = build
        self.password = password
        self.seats = {}
        server.cmd_join(self.client, [self.room])
        server.cmd_mode(self.client, [self.room, '+kl', password, '8'])

    def listing(self, closed=False):
        fields = super().listing(closed).split(';')
        fields[5] = '01%s00' % ('1' if closed else '0')
        return ';'.join(fields)

    def send(self, command, arguments):
        self.server.cmd_notice(self.client, [self.room, '\x01%s %s\x01' % (command, arguments)])

    def send_players(self):
        rows = ['%s,-1,0,-1,-1,H' % escape(self.client.nick)]
        for nick, seat in self.seats.items():
            rows.append('%s,%s,%s' % (escape(nick), ','.join(str(v) for v in seat['setup']), 'R' if seat['ready'] else ''))
        self.send('OYRPLAYERS', '|'.join(rows))

    def receive(self, line):
        message = parse(line)
        if not message:
            return
        prefix, command, params = message
        sender = prefix.split('!')[0]
        if command == 'NOTICE' and len(params) == 2 and fold(params[0]) == fold(self.room):
            name, arguments = ctcp(params[1])
            if name == 'OYRHELLO':
                if arguments != '1;' + escape(self.build):
                    self.send('OYRREFUSE', '%s;%s' % (escape(sender), escape('This room runs OpenYR %s.' % self.build)))
                    self.server.cmd_kick(self.client, [self.room, sender, 'Wrong build'])
                    return
                self.seats[sender] = {'setup': [-1, len(self.seats) + 1, -1, -1], 'ready': False}
                self.send('OYROPTS', 'Map=%s;MapName=%s;Credits=10000' % (escape('lakeblitz.map'), escape(self.map_name)))
                self.send_players()
            elif name == 'OYRREQ' and sender in self.seats:
                values = [int(v) for v in arguments.split(',')[:4]]
                self.seats[sender] = {'setup': values, 'ready': False}
                self.send_players()
            elif name == 'OYRREADY' and sender in self.seats:
                self.seats[sender]['ready'] = arguments == '1'
                self.send_players()
        elif command in ('PART', 'QUIT') and sender in self.seats:
            del self.seats[sender]
            self.send_players()
        else:
            super().receive(line)


class OpenYRGuestBot(Bot):
    """Joins the room of any game a player announces, using the password, then says hello and
    gets ready whenever the host's player list shows it not ready."""

    def __init__(self, server, nick, game_version, build, password):
        super().__init__(server, nick, game_version, hosts=False)
        self.build = build
        self.password = password
        self.rooms = set()

    def receive(self, line):
        message = parse(line)
        if not message:
            return
        prefix, command, params = message
        sender = prefix.split('!')[0]
        if command == 'NOTICE' and len(params) == 2:
            name, arguments = ctcp(params[1])
            if name == 'GAME' and fold(params[0]) == fold(BROADCAST):
                fields = arguments.split(';')
                host = self.server.clients.get(fold(sender))
                if len(fields) == 14 and host is not None and host.bot is None and fields[5][2:3] != '1':
                    room = fields[3]
                    if fold(room) not in self.rooms:
                        self.rooms.add(fold(room))
                        self.server.cmd_join(self.client, [room, self.password])
                        self.server.cmd_notice(self.client, [room, '\x01OYRHELLO 1;%s\x01' % escape(self.build)])
            elif name == 'OYRPLAYERS':
                for row in arguments.split('|'):
                    fields = row.split(',')
                    if len(fields) >= 6 and fold(fields[0]) == fold(self.client.nick) and 'R' not in fields[5]:
                        self.server.cmd_notice(self.client, [params[0], '\x01OYRREADY 1\x01'])
        elif command == 'KICK' and len(params) >= 2 and fold(params[1]) == fold(self.client.nick):
            self.rooms.discard(fold(params[0]))
        else:
            super().receive(line)


async def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=6667)
    parser.add_argument('--bots', type=int, default=3)
    parser.add_argument('--game-version', default='1.001')
    parser.add_argument('--log', default='')
    parser.add_argument('--openyr-build', default='')
    parser.add_argument('--room-password', default='')
    args = parser.parse_args()

    server = Server(args.log)

    async def connection(reader, writer):
        client = Client(server, writer)
        try:
            while True:
                data = await asyncio.wait_for(reader.readline(), timeout=300)
                if not data:
                    break
                server.handle(client, data.decode('utf-8', 'replace').rstrip('\r\n'))
                await writer.drain()
        except (asyncio.TimeoutError, ConnectionError):
            pass
        finally:
            server.drop(client, 'Connection closed')

    listener = await asyncio.start_server(connection, '127.0.0.1', args.port)
    server.bots = []
    bots = [Bot(server, name, args.game_version, hosts=index % 2 == 0, greets=index == 0)
            for index, name in enumerate(['Tanya', 'Yuri_Fan', 'Boris', 'Lt_Eva', 'Kirov'][:args.bots])]
    if args.openyr_build and args.room_password:
        bots.append(OpenYRHostBot(server, 'Eva_Host', args.game_version, args.openyr_build, args.room_password))
        bots.append(OpenYRGuestBot(server, 'Dasha_Guest', args.game_version, args.openyr_build, args.room_password))
    server.bots = bots
    print('CnCNet stand-in listening on 127.0.0.1:%d with %d bots' % (args.port, len(bots)), flush=True)
    async with listener:
        await asyncio.gather(listener.serve_forever(), *(bot.run() for bot in bots))


if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
