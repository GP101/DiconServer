Winsock I/O Model Samples
=========================

This directory contains samples illustrating the various Winsock I/O models
under the following directories:

    Directory       Description
    =========       ===========
    blocking        Illustrates the blocking sockets model. This includes
                    blocking client and server samples.

    nonblocking     Illustrates the non-blocking socket model using the
                    select API. Only a server sample is provided.

    WSAAsyncSelect  Illustrates the WSAAsyncSelect socket model. Only a
                    server sample is provided.

    WSAEventSelect  Illustrates the WSAEventSelect socket model. Only a
                    server sample is provided.

    overlapped      Illustrates the overlapped IO model using events.
                    Only a server sample is provided.

    iocp            Illustrates overlapped IO using an IO comletion port.
                    Both a client and server sample are provided.

All samples operate over both IPv4 and IPv6. The samples are echo samples.
The servers and blocking client can handle TCP and UDP while the completion
port client which operates only over TCP.
