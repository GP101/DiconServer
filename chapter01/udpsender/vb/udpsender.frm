VERSION 5.00
Begin VB.Form frmPeerB 
   Caption         =   "UdpSender"
   ClientHeight    =   3405
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   8865
   LinkTopic       =   "Form1"
   ScaleHeight     =   3405
   ScaleWidth      =   8865
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdDoIt 
      Caption         =   "Click to send a message"
      Height          =   735
      Left            =   120
      TabIndex        =   6
      Top             =   1920
      Width           =   1815
   End
   Begin VB.TextBox txtRemotePeer 
      Height          =   285
      Left            =   240
      TabIndex        =   3
      Text            =   "weipiii"
      Top             =   360
      Width           =   1815
   End
   Begin VB.TextBox txtRemotePort 
      Height          =   285
      Left            =   240
      TabIndex        =   2
      Text            =   "5150"
      Top             =   1200
      Width           =   1695
   End
   Begin VB.ListBox List1 
      Height          =   1815
      Left            =   2400
      TabIndex        =   1
      Top             =   840
      Width           =   6015
   End
   Begin VB.Label Label3 
      Caption         =   "Remote peer"
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   0
      Width           =   1095
   End
   Begin VB.Label Label4 
      Caption         =   "Remote port: "
      Height          =   375
      Left            =   240
      TabIndex        =   4
      Top             =   840
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "Status"
      Height          =   375
      Left            =   2400
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
Dim RetMsg As String



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
        ' Setup the remote host and port
        '
        remote_addr.sin_family = AF_INET     ' address family, internet: 2
        remote_addr.sin_port = htons(CLng(txtRemotePort.Text))
        remote_addr.sin_addr = GetHostByNameAlias(txtRemotePeer.Text)
        
        RetMsg = "Hello"
        ' Send the data
        dwRc = sendto(msg_sock, ByVal RetMsg, Len(RetMsg), 0, remote_addr, LenB(remote_addr))
        If dwRc = SOCKET_ERROR Then
            MsgBox "sendto failed. Error: " & Err.LastDllError
        Else
            List1.AddItem "We successfully sent " & dwRc & " bytes to " & _
             txtRemotePeer.Text & ":" & htons(remote_addr.sin_port)
        End If
    End If
    
    List1.AddItem "Closing the udp socket..."
    closesocket (msg_sock)
    msg_sock = INVALID_SOCKET
   
    cmdDoIt.Enabled = True
   
End Sub
