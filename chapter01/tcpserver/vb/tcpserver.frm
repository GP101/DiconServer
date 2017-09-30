VERSION 5.00
Begin VB.Form TcpServer 
   Caption         =   "TCP Sever"
   ClientHeight    =   3165
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8190
   LinkTopic       =   "Form1"
   ScaleHeight     =   3165
   ScaleWidth      =   8190
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox List1 
      Height          =   2010
      Left            =   3360
      TabIndex        =   3
      Top             =   720
      Width           =   4095
   End
   Begin VB.TextBox txtPort 
      Height          =   375
      Left            =   960
      TabIndex        =   2
      Text            =   "5150"
      Top             =   240
      Width           =   1215
   End
   Begin VB.CommandButton cmdDoIt 
      Caption         =   "start server"
      Enabled         =   0   'False
      Height          =   735
      Left            =   480
      TabIndex        =   0
      Top             =   1080
      Width           =   1695
   End
   Begin VB.Label Label2 
      Caption         =   "Status"
      Height          =   375
      Left            =   3360
      TabIndex        =   4
      Top             =   240
      Width           =   1095
   End
   Begin VB.Label Label1 
      Caption         =   "Port:"
      Height          =   255
      Left            =   480
      TabIndex        =   1
      Top             =   360
      Width           =   375
   End
End
Attribute VB_Name = "TcpServer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'
' Subroutine: Form_Load
'
' Description:
'    This routine makes sure the Winsock DLL has been properly loaded
'    in addition to setting the form up in its initial state.
'
Private Sub Form_Load()

    If TCPIPStartup Then
        cmdDoIt.Enabled = True
    Else
        cmdDoIt.Enabled = False
        MsgBox "Windows Sockets not initialized. Error: " & Err.LastDllError
    End If
End Sub

'
' Subroutine: Form_Unload
'
' Description:
'    This routine unloads the Winsock DLL.
'
Private Sub Form_Unload(Cancel As Integer)
    TCPIPShutDown
End Sub


Private Sub cmdDoIt_Click()
   Dim addr As sockaddr, client_addr As sockaddr, addrlen As Long, client_addrlen As Long, pSockAddr As Long
   Dim msg_sock As Long, accept_sock As Long, msgstr As String
   Dim i As Long
   
   List1.Clear
   
   cmdDoIt.Enabled = False
   
   'get socket handle
   accept_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
   
   If accept_sock = INVALID_SOCKET Then
       MsgBox "Couldn't create socket(). Error:" & Err.LastDllError, "Accept"
       cmdDoIt.Enabled = True
       Exit Sub
   End If
   
   '
   ' Setup our local address to listen on
   '
   addr.sin_family = AF_INET
   addr.sin_port = htons(CLng(txtPort.Text))
   addr.sin_addr = INADDR_ANY
      
   'bind to socket
   If bind(accept_sock, addr, Len(addr)) = SOCKET_ERROR Then
       MsgBox "Couldn't bind() to socket. Error: " & Err.LastDllError
       closesocket (accept_sock)
       cmdDoIt.Enabled = True
       Exit Sub
   End If

   'start listening on socket
   If listen(accept_sock, 1) = SOCKET_ERROR Then
       MsgBox "Couldn't listen() to socket. Error: " & Err.LastDllError
       closesocket (accept_sock)
       cmdDoIt.Enabled = True
       Exit Sub
   End If
   
   List1.AddItem "We are awaiting a connection on port " & txtPort
   
   ' The accept below is a blocking call, we must
   ' give the thread a chance to paint the list box
   ' before the thread blocks.
   DoEvents
   
   client_addrlen = Len(client_addr)
   '
   ' Wait for a client to connect to us.
   '
   msg_sock = accept(accept_sock, client_addr, client_addrlen)
   If msg_sock = INVALID_SOCKET Then
       MsgBox "Couldn't accept an socket connection. Error: " & Err.LastDllError
       closesocket (accept_sock)
       cmdDoIt.Enabled = True
       Exit Sub
   End If
   
   ' At this point you can do two things with these sockets. Await
   ' for more connections by calling accept again on ListeningSocket
   ' and start sending or receiving data on NewConnection. For
   ' simplicity We will stop listening for more connections by closing
   ' ListeningSocket. We will start sending and receiving data on
   ' NewConnection.
   closesocket (accept_sock)

   List1.AddItem "We are waiting to receive data..."
  
   Dim RetMsg As String
   Dim dwRc As Long
   RetMsg = String(7000, 0)
   dwRc = recv(msg_sock, ByVal RetMsg, 7000, 0)
   If dwRc = SOCKET_ERROR Then
         MsgBox "Couldn't recieve data from remote Socket. Error: " & Err.LastDllError
   End If
   
   List1.AddItem "We successfully received " & dwRc & " bytes."
   List1.AddItem "We are now going to close the client connection."

   closesocket (msg_sock)
   cmdDoIt.Enabled = True
   
                 
End Sub


