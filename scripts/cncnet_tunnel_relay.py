#!/usr/bin/env python3
"""Local stand-in for the CnCNet V3 tunnel bridge and relay, for two-copy tests on 127.0.0.1.

Run it with the isolated flag, as in:

    python -I scripts/cncnet_tunnel_relay.py --relay-port 24973 --player 1001:24971 --player 1002:24972

Bridge, one per --player ID:PORT:
  - Listens on 127.0.0.1:PORT. The game's [Tunnel] Port names that port.
  - Reads a game datagram as [sender u16 BE][receiver u16 BE][payload]. The receiver at offset 2
    is matched to another player's ID, compared as its low 16 bits.
  - Sends [local id u32 LE][peer id u32 LE][whole game datagram] to the relay. The IDs are the
    game's [Settings] Port and [OtherN] Port values.
  - Strips the 8-byte envelope from an inbound datagram and delivers the rest to the game's
    source endpoint, the address the game last sent from.
  - Drops a game datagram shorter than 4 bytes or with no known receiver.

Relay, one socket on 127.0.0.1:--relay-port:
  - Learns each local id's bridge endpoint from the source of its envelopes.
  - Forwards the whole envelope unchanged to the bridge of the peer id at offset 4.
  - Drops an envelope shorter than 8 bytes, an envelope longer than --relay-buffer (1024 bytes by
    default, the size the CnCNet server source reads into), and an envelope whose peer id it has not learned.
  - --latency-ms, --jitter-ms and --loss apply to this relay hop only. Each datagram crosses it once.

With no --player, only the relay runs. OpenYR's built-in lobby uses it that way, since each copy
of the game then runs its own bridge.

Not emulated: the CNCNET negotiation packets, the server's registration step, and V2 (a 4-byte
header sent straight to a server that allocates IDs over HTTP). The relay learns endpoints from
envelopes, so a player's first datagram is lost until that player has sent something.

Prints a stats line every --stats-interval seconds and a summary when it stops. Stop it by its
process ID; a Ctrl+C also prints the summary. --self-test checks the routing without a game.
"""

import argparse
import collections
import functools
import heapq
import itertools
import random
import selectors
import socket
import struct
import sys
import time

LOOPBACK = "127.0.0.1"
ENVELOPE_BYTES = 8
GAME_HEADER_BYTES = 4
RELAY_BUFFER_BYTES = 1024
MAX_DATAGRAM = 65535


class Tunnel:
    def __init__(self, players, relay_port, latency_ms=0.0, jitter_ms=0.0, loss_percent=0.0, seed=1,
                 relay_buffer=RELAY_BUFFER_BYTES):
        # players is a list of (tunnel id, bridge port). A bridge port is never the relay port.
        self.relay_addr = (LOOPBACK, relay_port)
        self.relay_buffer = relay_buffer
        self.ids = [pid & 0xFFFFFFFF for pid, _ in players]
        self.ids16 = [pid & 0xFFFF for pid, _ in players]
        self.latency_ms = latency_ms
        self.jitter_ms = jitter_ms
        self.loss_percent = loss_percent
        self.rng = random.Random(seed)
        self.counts = collections.Counter()
        self.max_envelope = 0
        self.max_game_datagram = 0
        self.game_sizes = collections.Counter()
        self.learned = {}
        self.game_endpoint = [None] * len(players)
        self.pending = []
        self.sequence = itertools.count()
        self.sel = selectors.DefaultSelector()
        self.game = []
        self.tunnel = []
        self.relay = self._udp(relay_port)
        self.sel.register(self.relay, selectors.EVENT_READ, self._on_relay)
        for index, (_, port) in enumerate(players):
            game_sock = self._udp(port)
            tunnel_sock = self._udp(0)
            self.game.append(game_sock)
            self.tunnel.append(tunnel_sock)
            self.sel.register(game_sock, selectors.EVENT_READ, functools.partial(self._on_game, index))
            self.sel.register(tunnel_sock, selectors.EVENT_READ, functools.partial(self._on_tunnel, index))

    @staticmethod
    def _udp(port):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((LOOPBACK, port))
        sock.setblocking(False)
        return sock

    def close(self):
        for sock in [self.relay] + self.game + self.tunnel:
            self.sel.unregister(sock)
            sock.close()
        self.sel.close()

    def _send(self, sock, data, dest):
        try:
            sock.sendto(data, dest)
        except OSError:
            self.counts["send_error"] += 1

    def _peer_index(self, index, receiver16):
        for other in range(len(self.ids16)):
            if other != index and self.ids16[other] == receiver16:
                return other
        return None

    def _on_game(self, index):
        try:
            data, addr = self.game[index].recvfrom(MAX_DATAGRAM)
        except OSError:
            self.counts["game_socket_error"] += 1
            return
        self.game_endpoint[index] = addr
        self.max_game_datagram = max(self.max_game_datagram, len(data))
        self.game_sizes[len(data)] += 1
        if len(data) < GAME_HEADER_BYTES:
            self.counts["bridge_short"] += 1
            return
        receiver = struct.unpack_from(">H", data, 2)[0]
        peer = self._peer_index(index, receiver)
        if peer is None:
            self.counts["bridge_unknown_receiver"] += 1
            return
        envelope = struct.pack("<II", self.ids[index], self.ids[peer]) + data
        self._send(self.tunnel[index], envelope, self.relay_addr)
        self.counts["bridge_out"] += 1

    def _on_tunnel(self, index):
        try:
            data, _ = self.tunnel[index].recvfrom(MAX_DATAGRAM)
        except OSError:
            self.counts["tunnel_socket_error"] += 1
            return
        if len(data) < ENVELOPE_BYTES:
            self.counts["inbound_short"] += 1
            return
        _, peer_id = struct.unpack_from("<II", data, 0)
        if peer_id != self.ids[index]:
            self.counts["inbound_misrouted"] += 1
            return
        endpoint = self.game_endpoint[index]
        if endpoint is None:
            self.counts["inbound_no_game_endpoint"] += 1
            return
        self._send(self.game[index], data[ENVELOPE_BYTES:], endpoint)
        self.counts["delivered"] += 1

    def _on_relay(self):
        try:
            data, addr = self.relay.recvfrom(MAX_DATAGRAM)
        except OSError:
            self.counts["relay_socket_error"] += 1
            return
        self.max_envelope = max(self.max_envelope, len(data))
        if len(data) > self.relay_buffer:
            self.counts["relay_oversize"] += 1
            return
        if len(data) < ENVELOPE_BYTES:
            self.counts["relay_short"] += 1
            return
        local_id, peer_id = struct.unpack_from("<II", data, 0)
        self.learned[local_id] = addr
        dest = self.learned.get(peer_id)
        if dest is None:
            self.counts["relay_unknown_peer"] += 1
            return
        if self.loss_percent > 0 and self.rng.random() * 100.0 < self.loss_percent:
            self.counts["relay_lost"] += 1
            return
        jitter = self.rng.uniform(-self.jitter_ms, self.jitter_ms) if self.jitter_ms > 0 else 0.0
        delay = max(0.0, (self.latency_ms + jitter) / 1000.0)
        heapq.heappush(self.pending, (time.monotonic() + delay, next(self.sequence), data, dest))
        self.counts["relay_in"] += 1

    def pump(self, timeout):
        """Handles ready sockets and due relay sends for up to timeout seconds."""
        now = time.monotonic()
        wait = timeout
        if self.pending:
            wait = min(wait, max(0.0, self.pending[0][0] - now))
        for key, _ in self.sel.select(wait):
            key.data()
        now = time.monotonic()
        while self.pending and self.pending[0][0] <= now:
            _, _, data, dest = heapq.heappop(self.pending)
            self._send(self.relay, data, dest)
            self.counts["relay_forwarded"] += 1

    def summary(self):
        keys = sorted(self.counts)
        parts = [f"{key}={self.counts[key]}" for key in keys]
        parts.append(f"max_game_datagram={self.max_game_datagram}")
        parts.append(f"max_envelope={self.max_envelope}")
        return " ".join(parts)

    def size_report(self):
        """The most common game datagram lengths, as length:count pairs."""
        return " ".join(f"{size}:{count}" for size, count in self.game_sizes.most_common(12))


def _player(text):
    if ":" not in text:
        raise argparse.ArgumentTypeError(f"'{text}' is not ID:PORT")
    ident, port = text.split(":", 1)
    try:
        value = int(ident)
        port_number = int(port)
    except ValueError:
        raise argparse.ArgumentTypeError(f"'{text}' is not ID:PORT")
    if value == 0 or not -65535 <= value <= 65535:
        raise argparse.ArgumentTypeError(f"tunnel id {value} is not a nonzero number from -65535 to 65535")
    if not 1024 <= port_number <= 65535:
        raise argparse.ArgumentTypeError(f"port {port_number} is outside 1024 to 65535")
    return value, port_number


def _percent(text):
    value = float(text)
    if not 0.0 <= value <= 100.0:
        raise argparse.ArgumentTypeError("loss must be from 0 to 100")
    return value


def _run(args):
    players = args.player
    ports = [port for _, port in players] + [args.relay_port]
    if len(set(ports)) != len(ports):
        print("relay: the relay port and every bridge port must differ", flush=True)
        return 1
    ids16 = [pid & 0xFFFF for pid, _ in players]
    if len(set(ids16)) != len(ids16):
        print("relay: two players share a tunnel number in its low 16 bits", flush=True)
        return 1

    try:
        tunnel = Tunnel(players, args.relay_port, args.latency_ms, args.jitter_ms, args.loss, args.seed,
                        args.relay_buffer)
    except OSError as error:
        print(f"relay: cannot bind 127.0.0.1 ({error})", flush=True)
        return 1

    bridges = ", ".join(f"id {pid} on {port}" for pid, port in players) or "none"
    print(f"relay: bridges {bridges}; relay on {args.relay_port}; latency {args.latency_ms} ms, "
          f"jitter {args.jitter_ms} ms, loss {args.loss}%, seed {args.seed}, relay buffer {args.relay_buffer} bytes",
          flush=True)
    print("relay: ready", flush=True)

    started = time.monotonic()
    next_stats = started + args.stats_interval
    end = started + args.duration if args.duration > 0 else None
    try:
        while end is None or time.monotonic() < end:
            tunnel.pump(0.05)
            now = time.monotonic()
            if now >= next_stats:
                print(f"relay: t={now - started:.1f}s {tunnel.summary()}", flush=True)
                next_stats = now + args.stats_interval
    except KeyboardInterrupt:
        pass
    print(f"relay: stopped after {time.monotonic() - started:.1f}s; {tunnel.summary()}", flush=True)
    print(f"relay: game datagram lengths {tunnel.size_report()}", flush=True)
    tunnel.close()
    return 0


def _self_test():
    failures = []

    def check(condition, what):
        print(("ok      " if condition else "FAILED  ") + what, flush=True)
        if not condition:
            failures.append(what)

    def send(sock, sender, receiver, payload, port):
        sock.sendto(struct.pack(">HH", sender & 0xFFFF, receiver & 0xFFFF) + payload, (LOOPBACK, port))

    def drain(tunnel, seconds):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            tunnel.pump(0.01)

    def receive(sock):
        try:
            return sock.recvfrom(MAX_DATAGRAM)
        except BlockingIOError:
            return None, None

    def discard(sock):
        while receive(sock)[0] is not None:
            pass

    def fake_game():
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((LOOPBACK, 0))
        sock.setblocking(False)
        return sock

    def warm_up(tunnel, game_a, game_b, id_a, id_b, port_a, port_b):
        # Each player's first datagram teaches the relay its bridge endpoint. A's goes first,
        # so it is dropped for lack of a route; B's reaches A, and both are then known.
        send(game_a, id_a, id_b, b"warm", port_a)
        drain(tunnel, 0.2)
        send(game_b, id_b, id_a, b"warm", port_b)
        drain(tunnel, 0.6)
        discard(game_a)
        discard(game_b)

    base = 24971

    tunnel = Tunnel([(1001, base), (1002, base + 1)], base + 3, 0.0, 0.0, 0.0, 1)
    game_a, game_b = fake_game(), fake_game()
    try:
        warm_up(tunnel, game_a, game_b, 1001, 1002, base, base + 1)

        send(game_a, 1001, 1002, b"hello", base)
        drain(tunnel, 0.2)
        data, addr = receive(game_b)
        check(data == struct.pack(">HH", 1001, 1002) + b"hello" and addr[1] == base + 1,
              "A to B: the datagram arrives unchanged, from B's bridge port")
        discard(game_b)

        send(game_b, 1002, 1001, b"reply", base + 1)
        drain(tunnel, 0.2)
        data, addr = receive(game_a)
        check(data == struct.pack(">HH", 1002, 1001) + b"reply" and addr[1] == base,
              "B to A: the reply arrives unchanged, from A's bridge port")
        discard(game_a)

        send(game_a, 1001, 4242, b"nobody", base)
        drain(tunnel, 0.2)
        check(receive(game_b)[0] is None and tunnel.counts["bridge_unknown_receiver"] >= 1,
              "a receiver no player holds is dropped at the bridge")

        game_a.sendto(b"abc", (LOOPBACK, base))
        drain(tunnel, 0.2)
        check(tunnel.counts["bridge_short"] >= 1, "a datagram shorter than the 4-byte header is dropped")

        send(game_a, 1001, 1002, b"x" * 1100, base)
        drain(tunnel, 0.2)
        check(receive(game_b)[0] is None and tunnel.counts["relay_oversize"] >= 1,
              "an envelope over the 1024-byte relay buffer is dropped")
    finally:
        tunnel.close()
        game_a.close()
        game_b.close()

    tunnel = Tunnel([(1001, base), (1002, base + 1)], base + 3, 0.0, 0.0, 100.0, 1)
    game_a, game_b = fake_game(), fake_game()
    try:
        warm_up(tunnel, game_a, game_b, 1001, 1002, base, base + 1)
        send(game_a, 1001, 1002, b"lost", base)
        drain(tunnel, 0.2)
        check(receive(game_b)[0] is None and tunnel.counts["relay_lost"] >= 1,
              "100 percent loss drops every datagram on the relay hop")
    finally:
        tunnel.close()
        game_a.close()
        game_b.close()

    tunnel = Tunnel([(1001, base), (1002, base + 1)], base + 3, 300.0, 0.0, 0.0, 1)
    game_a, game_b = fake_game(), fake_game()
    try:
        warm_up(tunnel, game_a, game_b, 1001, 1002, base, base + 1)
        started = time.monotonic()
        send(game_a, 1001, 1002, b"late", base)
        drain(tunnel, 0.1)
        early = receive(game_b)[0] is None
        drain(tunnel, 0.8)
        data, _ = receive(game_b)
        elapsed = time.monotonic() - started
        check(early and data is not None and elapsed >= 0.28,
              "300 ms latency holds the datagram on the relay hop until it is due")
    finally:
        tunnel.close()
        game_a.close()
        game_b.close()

    tunnel = Tunnel([(-5, base), (1002, base + 1)], base + 3, 0.0, 0.0, 0.0, 1)
    game_a, game_b = fake_game(), fake_game()
    try:
        warm_up(tunnel, game_a, game_b, -5, 1002, base, base + 1)
        send(game_b, 1002, -5, b"neg", base + 1)
        drain(tunnel, 0.2)
        data, _ = receive(game_a)
        check(data == struct.pack(">HH", 1002, 0xFFFB) + b"neg",
              "a negative tunnel number is matched by its low 16 bits")
    finally:
        tunnel.close()
        game_a.close()
        game_b.close()

    print("SELF-TEST PASSED" if not failures else f"SELF-TEST FAILED: {len(failures)} check(s)", flush=True)
    return 0 if not failures else 1


def main(argv):
    if "--self-test" in argv:
        return _self_test()

    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--relay-port", type=int, required=True, help="UDP port of the relay socket")
    parser.add_argument("--player", type=_player, action="append", default=[], metavar="ID:PORT",
                        help="a tunnel number and the bridge port its game's [Tunnel] Port names; "
                             "leave out every --player to run the relay alone, for games with their own bridge")
    parser.add_argument("--latency-ms", type=float, default=0.0, help="delay added to each relay hop")
    parser.add_argument("--jitter-ms", type=float, default=0.0,
                        help="random extra delay, from minus to plus this, on each relay hop; can reorder")
    parser.add_argument("--loss", type=_percent, default=0.0, help="percent of relay hops dropped")
    parser.add_argument("--seed", type=int, default=1, help="seed for jitter and loss")
    parser.add_argument("--relay-buffer", type=int, default=RELAY_BUFFER_BYTES,
                        help="largest envelope the relay accepts; the CnCNet server reads into 1024 bytes")
    parser.add_argument("--duration", type=float, default=0.0, help="seconds to run; 0 runs until stopped")
    parser.add_argument("--stats-interval", type=float, default=10.0, help="seconds between stats lines")
    args = parser.parse_args(argv)

    if not 1024 <= args.relay_port <= 65535:
        parser.error("--relay-port must be from 1024 to 65535")
    if not 1024 <= args.relay_buffer <= 65535:
        parser.error("--relay-buffer must be from 1024 to 65535")
    if args.latency_ms < 0 or args.jitter_ms < 0:
        parser.error("latency and jitter must not be negative")
    if len(args.player) == 1:
        parser.error("name at least two --player entries, or none to run the relay alone")
    return _run(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
