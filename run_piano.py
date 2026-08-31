#!/usr/bin/env python3
"""
Simple HTTP server to serve the Interactive Piano application
"""
import http.server
import socketserver
import os
import webbrowser
from pathlib import Path

PORT = 8000
SCRIPT_DIR = Path(__file__).parent

class PianoHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(SCRIPT_DIR), **kwargs)

    def end_headers(self):
        # Add headers to prevent caching
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate, max-age=0')
        super().end_headers()

def run_server():
    print(f"🎹 Interactive Piano Server")
    print(f"=" * 50)
    print(f"Opening browser at http://localhost:{PORT}/piano.html")
    print(f"Press Ctrl+C to stop the server")
    print(f"=" * 50)
    
    # Try to open browser
    try:
        webbrowser.open(f'http://localhost:{PORT}/piano.html')
    except:
        print(f"Could not auto-open browser. Visit http://localhost:{PORT}/piano.html manually")
    
    with socketserver.TCPServer(("", PORT), PianoHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n\n🛑 Server stopped")

if __name__ == "__main__":
    run_server()
