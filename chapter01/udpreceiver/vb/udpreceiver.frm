VERSION 5.00
Begin VB.Form frmPeerB 
   Caption         =   "UdpReceiver"
   ClientHeight    =   3405
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8565
   LinkTopic       =   "Form1"
   ScaleHeight     =   3405
   ScaleWidth      =   8565
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox List1 
      Height          =   1815
      Left            =   2280
      TabIndex        =   2
      Top             =   840
      Width           =   5655
   End
   Begin VB.CommandButton cmdDoIt 
      Caption         =   "Bind to port 5150 and ready to receive"
      Height          =   735
      Left            =   240
      TabIndex        =   1
      Top             =   480
      Width           =   1575
   End
   Begin VB.Label Label2 
      Caption         =   "Status"
      Height          =   375
      Left            =   2280
      TabIndex        =   0
      Top             =   360
      Width           =   855
   End
End
Attribute VB_Name = "frmPeerB"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim msg_sock As Long, msgstr As String
Dim addr As sockaddr, remote_addr As sockaddr, from_addr As sockaddr
Dim fromlen As Long
Dim dwRc As Long
Dim RetMsg As String, RemoteAddress As String


'
' Subroutine: Form_Load
'
' Description:
'    This routine initializes Winsock as well as the form.
'
Private Sub Form_Load()
    If TCPIPStartup Then
        cmdDoIt.Enabled = True
    Else
        cmdDoIt.Enabled = False
        MsgBox "Windows Sockets not initialized. Error: " & Err.LastDllError
    End If
    msg_sock = INVALID_SOCKET
    
End Sub

'
' Subroutine: Form_Unload
'
' Description:
'    This routine unloads Winsock in addition to freeing any resources.
'
Private Sub Form_Unload(Cancel As Integer)
    If msg_sock <> INVALID_SOCKET Then
        closesocket msg_sock
    End If
    TCPIPShutDown
End Sub



'
' Subroutine: CmdBind_Click
'
' Description:
'    This routines is triggered by the "Bind" button. When the user hits
'    this button, we bind to the specified local port. This is for receiving
'    datagrams as we will only receive messaged send to this port number.
'
Private Sub CmdDoIt_Click()

   List1.Clear
   cmdDoIt.Enabled = False
   'get socket handle
   msg_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)
   
   If msg_sock = INVALID_SOCKET Then
       MsgBox "Couldn't create socket(). Error: " & Err.LastDllError
   Else
       ' Setup the local address to listen for UDP datagrams
       '
       addr.sin_family = AF_INET
       addr.sin_port = htons(CLng(5150))
       addr.sin_addr = INADDR_ANY
              
       'bind to socket
       If bind(msg_sock, addr, Len(addr)) = SOCKET_ERROR Then
           MsgBox "Couldn't bind() to socket locally. Error: " & Err.LastDllError
       Else
           List1.AddItem "We are ready to receive 1 datagram from any interface on port 5150..."
           
           'The recvfrom below is a blocking call.
           'Call DoEvents to give the app a chance to refresh
           DoEvents
           
           RetMsg = String(7000, 0)
           fromlen = LenB(from_addr)
           dwRc = recvfrom(msg_sock, ByVal RetMsg, 7000, 0, from_addr, fromlen)
           If dwRc = SOCKET_ERROR Then
                MsgBox "Couldn't recieve data from remote Socket. Error: " & Err.LastDllError
           Else
                RemoteAddress = String(50, 0)
                lstrcpy1 RemoteAddress, inet_ntoa(from_addr.sin_addr)
                
                List1.AddItem "We successfully received " & dwRc & _
                 " bytes from address " & Left(RemoteAddress, InStr(RemoteAddress, Chr(0)) - 1) & ":" & CStr(ntohs(from_addr.sin_port)) & "."
           End If
      
       End If
    End If
    
    List1.AddItem "Closing the udp socket..."
    closesocket (msg_sock)
    msg_sock = INVALID_SOCKET
   
    cmdDoIt.Enabled = True
   
End Sub
