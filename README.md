# ConcurrentFileTransfer
Created a multi-threaded file transfer application over TCP that can efficiently receive data from multiple clients.

### Steps to run

```
g++ -std=c++17 client.cc -o client
```


```
g++ -std=c++17 server.cc -o server
```

- Start the server by running ./server
- Open up multiple terminal windows and run ./client x where x is the file number the client wants to send.

