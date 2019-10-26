require "socket"

server=TCPServer.open(50000)
socket=server.accept
socket.puts("5,3")
puts socket.gets
socket.close
a.psh "3"
