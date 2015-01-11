'
' nfc_sample_01.vb
' Copyright 2009,2011 Sony Corporation
'
Imports System
Imports System.Text
Imports System.Threading
Imports System.Runtime.InteropServices
Imports System.IO
Imports System.Windows.Forms

Module nfc_sample_01
    Private Const EXIT_FAILED As Integer = 1
    Private Const EXIT_SUCCEED As Integer = 0
    Private Const BUFSIZ As Integer = 512

    Sub Main()
        Dim lw As ListenerWindow = New ListenerWindow
        Dim msg_str_of_find As String = "find"
        Dim msg_str_of_enable As String = "enable"
        Dim bRet As Boolean = False

        Dim card_find_message As UInt32 = RegisterWindowMessage(msg_str_of_find)
        If (card_find_message.Equals(Convert.ToUInt32(0))) Then
            Console.Write("Failed: RegisterWindowMessage")
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Dim card_enable_message As UInt32 = RegisterWindowMessage(msg_str_of_enable)
        If (card_enable_message.Equals(Convert.ToUInt32(0))) Then
            Console.Write("Failed: RegisterWindowMessage")
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        bRet = lw.WatchMessage(card_find_message)
        If (bRet = False) Then
            Console.WriteLine("Failed: WatchMessage")
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        lw.WatchMessage(card_enable_message)
        If (bRet = False) Then
            Console.WriteLine("Failed: WatchMessage")
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Dim messageHandler As MessageHandler = New MessageHandler(card_find_message, card_enable_message)
        AddHandler lw.handler, AddressOf messageHandler.MessageHandlerFunc

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Initialize()
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Initialize")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Dim port_name As StringBuilder = New StringBuilder("USB0")
        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Open(port_name)
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Open")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Set_Poll_Callback_Parameters( _
            lw.Handle, _
            msg_str_of_find, _
            msg_str_of_enable)
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Set_Poll_Callback_Parameters")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Dim DEVICE_TYPE_NFC_14443A_18092_106K As Integer = &H1
        Dim target_device As UInt32 = Convert.ToUInt32(DEVICE_TYPE_NFC_14443A_18092_106K)
        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Start_Poll_Mode(target_device)
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Start_Poll_Mode")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Application.Run(lw)

        If (messageHandler.bRet = False) Then
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Dim command_packet_data() As Byte = {&H30, &H0}   ' Command for Mifare ultralight, &H30: 16byte Reading command, &H0: Start address
        Dim command_packet_data_length As UInt16 = Convert.ToUInt16(2)
        Dim response_packet_data(BUFSIZ) As Byte
        Dim response_packet_data_length As UInt16

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Thru(command_packet_data, _
            command_packet_data_length, _
            response_packet_data, _
            response_packet_data_length)

        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Thru")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        For idx As Integer = 0 To 15
            Console.WriteLine("response_packet_data[{0}]: 0x{1:X2}", _
                idx, _
                response_packet_data(idx))
        Next

        Dim RE_NOTIFICATION_SAME_DEVICE As UInt32 = Convert.ToUInt32(&H0)
        Dim stop_mode As UInt32 = Convert.ToUInt32(RE_NOTIFICATION_SAME_DEVICE)

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Stop_Dev_Access(stop_mode)
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Stop_Dev_Access")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If


        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Stop_Poll_Mode()
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Stop_Poll_Mode")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Close()
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Close")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Uninitialize()
        If (bRet = False) Then
            Console.WriteLine("Failed: FeliCaLib_Nfc_Uninitialize")
            ErrorRoutine()
            Environment.ExitCode = EXIT_FAILED
            Return
        End If

        Console.WriteLine("Succeed.")
        Environment.ExitCode = EXIT_SUCCEED
        Return
    End Sub

    Public Declare Function RegisterWindowMessage _
                Lib "User32.dll" _
                Alias "RegisterWindowMessageA" ( _
                    ByVal lpString As String) As UInt32

    Public Sub ErrorRoutine()
        Dim error_info() As UInt32 = {Convert.ToUInt32(0), Convert.ToUInt32(0)}

        FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Get_Last_Error(error_info)
        Console.Write("error_info[0]: 0x{0:X8}" + vbLf + "error_info[1]: 0x{1:X8}" + vbLf, error_info(0), error_info(1))
        FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Close()
        FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Uninitialize()
        Return
    End Sub

    Public Delegate Sub MessageReceivedEventHandler(ByVal sender As Object, ByVal m As MessageReceivedEventArgs)

    Public Class MessageReceivedEventArgs : Inherits EventArgs
        Private ReadOnly _message As Message
        Public Sub New(ByVal message As Message)
            _message = message
        End Sub
        Public ReadOnly Property Message() As Message
            Get
                Return _message
            End Get
        End Property
    End Class

    Public Class ListenerWindow : Inherits Form
        Private Const MAX_MESSAGES As Integer = 2
        Public Event handler As MessageReceivedEventHandler
        Private messageSet As UInt32() = New UInt32(MAX_MESSAGES - 1) {}
        Private registeredMessage As Integer = 0
        Public Function WatchMessage(ByVal message As UInt32) As Boolean
            If registeredMessage < messageSet.Length Then
                messageSet(registeredMessage) = message
                registeredMessage = registeredMessage + 1
                Return True
            Else
                Return False
            End If

        End Function
        Protected Overrides ReadOnly Property CreateParams() As CreateParams
            Get
                Const WS_EX_TOOLWINDOW As Int32 = &H80
                Const WS_POPUP As Int64 = &H80000000
                Const WS_VISIBLE As Int32 = &H10000000
                Const WS_SYSMENU As Int32 = &H80000
                Const WS_MAXIMIZEBOX As Int32 = &H10000

                Dim cp As CreateParams
                cp = MyBase.CreateParams
                cp.ExStyle = WS_EX_TOOLWINDOW
                cp.Style = WS_POPUP Or WS_VISIBLE Or WS_SYSMENU Or WS_MAXIMIZEBOX
                cp.Width = 0
                cp.Height = 0
                Return cp
            End Get
        End Property
        Protected Overrides Sub WndProc(ByRef m As System.Windows.Forms.Message)
            Dim handleMessage As Boolean = False
            For i As Integer = 0 To registeredMessage - 1
                If (Convert.ToUInt32(m.Msg).Equals(messageSet(i))) Then
                    handleMessage = True
                End If
            Next
            If (handleMessage And Not handlerEvent Is Nothing) Then
                RaiseEvent handler(DBNull.Value, New MessageReceivedEventArgs(m))
            End If
            MyBase.WndProc(m)
            Return
        End Sub
    End Class

    Public Class MessageHandler
        Public bRet As Boolean
        Private target_number As UInt32
        Private card_find_message As UInt32
        Private card_enable_message As UInt32

        Public Sub New(ByVal findMsg As UInt32, ByVal enableMsg As UInt32)
            card_find_message = findMsg
            card_enable_message = enableMsg
        End Sub
        Public Sub MessageHandlerFunc(ByVal sender As Object, ByVal e As MessageReceivedEventArgs)
            bRet = False
            If (Convert.ToUInt32(e.Message.Msg).Equals(card_find_message)) Then
                Dim pDevInfo As IntPtr = e.Message.LParam
                Dim pDeviceData_A As IntPtr
                If (IntPtr.Size = 8) Then
                    pDeviceData_A = New IntPtr(pDevInfo.ToInt64() + Marshal.OffsetOf(GetType(DEVICE_INFO), "dev_info").ToInt64())
                Else
                    pDeviceData_A = New IntPtr(pDevInfo.ToInt32() + Marshal.OffsetOf(GetType(DEVICE_INFO), "dev_info").ToInt32())
                End If

                Dim DeviceData_A As DEVICE_DATA_NFC_14443A_18092_106K = Marshal.PtrToStructure(pDeviceData_A, GetType(DEVICE_DATA_NFC_14443A_18092_106K))

                target_number = DeviceData_A.target_number
                bRet = FeliCaNfcDllWrapperBasic.FeliCaLib_Nfc_Start_Dev_Access(target_number)
                If (bRet = False) Then
                    Console.WriteLine("Failed: FeliCaLib_Nfc_Start_Dev_Access")
                    Application.Exit()
                    Return
                End If
            ElseIf (Convert.ToUInt32(e.Message.Msg).Equals(card_enable_message)) Then
                bRet = True
                Application.Exit()
                Return
            End If
            Return
        End Sub

    End Class

End Module
