import socketserver, time

class MyTCPHandler(socketserver.BaseRequestHandler):
    def handle(self):
        total_size = 0
        start_time = time.time()
        while True:
            data = self.request.recv(4096).strip()
            if not data:
                break
            total_size += len(data)
        print(start_time, total_size / (time.time() - start_time) / 1024 / 1024 * 8)

if __name__ == "__main__":
    HOST, PORT = "0.0.0.0", 80
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)
    server.serve_forever()