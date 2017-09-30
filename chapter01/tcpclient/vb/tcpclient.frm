VERSION 5.00
Begin VB.Form tcpclient 
   Caption         =   "TCP Client"
   ClientHeight    =   3015
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7605
   LinkTopic       =   "Form1"
   ScaleHeight     =   3015
   ScaleWidth      =   7605
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox List1 
      Height          =   1230
      Left            =   4080
      TabIndex        =   5
      Top             =   1320
      Width           =   2775
   End
   Begin VB.CommandButton cmdDoIt 
      Caption         =   "Click this button to connect, send ""hello"" to Server, and close the socket connection."
      Height          =   1575
      Left            =   360
      TabIndex        =   4
      Top             =   960
      Width           =   3015
   End
   Begin VB.TextBox txtPort 
      Height          =   285
      Left            =   5040
      TabIndex        =   3
      Text            =   "5150"
      Top             =   240
      Width           =   735
   End
   Begin VB.TextBox txtServer 
      Height          =   375
      Left            =   1800
      TabIndex        =   0
      Text            =   "weipiii"
      Top             =   240
      Width           =   1575
   End
   Begin VB.Label lblStatus 
      Caption         =   "Status"
      Height          =   255
      Left            =   4080
      TabIndex        =   6
      Top             =   960
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "Remote Port:"
      Height          =   375
      Left            =   3600
      TabIndex        =   2
      Top             =   240
      Width           =   1215
   End
   Begin VB.Label Label1 
      Caption         =   "Remote Server:"
      Height          =   255
      Left            =   240
      TabIndex        =   1
      Top             =   240
      Width           =   1335
   End
End
Attribute VB_Name = "tcpclient"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim soc As Long, dwRc As Long
Dim RemoteAddr As sockaddr
Dim RetMsg As String

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
    soc = INVALID_SOCKET
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
    
    List1.Clear
        
    soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
    If soc <> INVALID_SOCKET Then
        RemoteAddr.sin_family = AF_INET
        RemoteAddr.sin_port = htons(CLng(txtPort.Text))
        RemoteAddr.sin_addr = GetHostByNameAlias(txtServer.Text)
        
        'connect
        dwRc = connect(soc, RemoteAddr, sockaddr_size)
            
        If dwRc <> SOCKET_ERROR Then
            List1.AddItem "Our connection succeeded."
        Else
            MsgBox "Couldn't connect to remote Socket. Error: " & Err.LastDllError
            Exit Sub
        End If
        
        List1.AddItem "We will now try to send a hello message."
        RetMsg = "Hello"
        '
        ' send data
        dwRc = send(soc, ByVal RetMsg, Len(RetMsg), 0)
        If dwRc <> SOCKET_ERROR Then
           List1.AddItem "We successfully sent " & Len(RetMsg) & " bytes."
        Else
            MsgBox "Couldn't send data to remote Socket. Error: " & Err.LastDllError
        End If
        
        List1.AddItem "We are closing the connection"
        closesocket (soc)
        soc = INVALID_SOCKET
    Else
        MsgBox "Couldn't Create Socket. Error: " & Err.LastDllError
    End If
    
    
    
End Sub


