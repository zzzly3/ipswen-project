import socket

try:
      # Create a socket object
      server_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

      # Bind the socket to a specific IP address and port
      server_socket.bind(('::', 80))

      # Listen for incoming connections
      server_socket.listen()

      # Accept and handle client connections
      while True:
          try:
              client_socket, address = server_socket.accept()
              print(f"Accepted connection from: {address}")

              # Handle client request (add your custom logic here)

              # Close the client socket
              client_socket.close()

          except KeyboardInterrupt:
              print("Server stopped by the user")
              break

      # Close the server socket
      server_socket.close()

except OSError as e:
      print(f"Failed to create socket: {e}")
