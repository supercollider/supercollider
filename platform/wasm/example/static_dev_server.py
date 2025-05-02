from http.server import SimpleHTTPRequestHandler
import socketserver
import sys

class Handler(SimpleHTTPRequestHandler):
    def end_headers(self):
        # inject headers to allow SharedArrayBuffer, see
        # https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        SimpleHTTPRequestHandler.end_headers(self)

if __name__ == '__main__':
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8000
    host = str(sys.argv[2]) if len(sys.argv) > 2 else "127.0.0.1"
    with socketserver.TCPServer((host, port), Handler) as httpd:
        print(f"Serving on {host}:{port}")
        httpd.serve_forever()
