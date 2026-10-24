#!/usr/bin/env python3
"""Serves the benchmark page, which compares the sessions run.py saved.

    python3 langutils/sc_bench_inspect/serve.py [--port 8001] [--results dir]

Then open http://localhost:8001. Sessions are read from disk on each load, so a session run.py saves while this
serves shows up after a reload.
"""

import argparse
import json
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

from run import DEFAULT_RESULTS, HERE


def load_sessions(results):
    sessions = []
    for path in sorted(results.glob('*.json')):
        try:
            session = json.loads(path.read_text())
        except (OSError, json.JSONDecodeError) as e:
            print(f'Skipping {path}: {e}')
            continue
        session['id'] = path.stem
        sessions.append(session)
    return sessions


def make_handler(results):
    class Handler(SimpleHTTPRequestHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=str(HERE / 'web'), **kwargs)

        def do_GET(self):
            if self.path != '/api/sessions':
                super().do_GET()
                return
            body = json.dumps(load_sessions(results)).encode('utf-8')
            self.send_response(HTTPStatus.OK)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', str(len(body)))
            self.send_header('Cache-Control', 'no-store')
            self.end_headers()
            self.wfile.write(body)

        # Only errors.
        def log_request(self, code='-', size='-'):
            if isinstance(code, HTTPStatus):
                code = code.value
            if str(code)[0] != '2' and str(code) != '304':
                super().log_request(code, size)

    return Handler


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--port', type=int, default=8001)
    parser.add_argument('--results', type=Path, default=DEFAULT_RESULTS)
    args = parser.parse_args()

    server = ThreadingHTTPServer(('127.0.0.1', args.port), make_handler(args.results))
    print(f'Serving on http://localhost:{args.port}, sessions from {args.results}')
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
