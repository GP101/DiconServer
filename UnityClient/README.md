# Unity Client for TcpInteract C# server

Filename: NewUnity2019Project_UnityClient03_gameObject 20201118

https://github.com/AbleOpus/TcpInteract

# TcpInteract
TcpInteract is a simple, easy-to-use, TCP, .NET, networking library. It uses sockets directly (not any 3rd party service). No concern is needed for parsing/serializing data or implementing asynchronous logic. TcpInteract has a server-side client and a client-side client which both greatly abstract from using sockets directly. It has a simple client system where the clients connect, then send a login request. The request is then accepted or rejected by the server. The client sends a login package which contains its name. Names are used to distinguish clients – not socket handles.
