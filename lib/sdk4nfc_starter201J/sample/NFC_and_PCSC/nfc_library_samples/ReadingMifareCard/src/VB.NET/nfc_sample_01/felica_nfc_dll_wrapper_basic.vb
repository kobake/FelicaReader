'
' felica_nfc_dll_wrapper_basic.vb
' Copyright 2009,2011 Sony Corporation
'
Imports System
Imports System.Text
Imports System.Runtime.InteropServices

'structs
<StructLayout(LayoutKind.Sequential)> Public Structure DEVICE_DATA_NFC_14443A_18092_106K
    Dim target_number As UInt32
    Dim sens_res As UInt16
    Dim sel_ers As Byte
    Dim NFCID1_size As Byte
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
    Dim NFCID1 As Byte()
    Dim ATS_size As Byte
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
    Dim ATS As Byte()
End Structure

<StructLayout(LayoutKind.Sequential)> Public Structure DEVICE_DATA_NFC_14443B_106K
    Dim target_number As UInt32
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)> _
    Dim ATQB As Byte()
    Dim ATTRIB_size As Byte
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
    Dim ATTRIB As Byte()
End Structure

<StructLayout(LayoutKind.Sequential)> Public Structure DEVICE_DATA_NFC_18092_212_424K
    Dim id As Byte
    Dim target_nubmer As UInt32
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
    Dim NFCID2 As Byte()
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
    Dim Pad As Byte()
    Dim RD_size As Byte
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
    Dim RD As Byte()
End Structure

<StructLayout(LayoutKind.Sequential)> Public Structure DEVICE_INFO
    Dim target_device As UInt32
    <MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
    Dim dev_info As Byte()
End Structure

Public Class FeliCaNfcDllWrapperBasic
    'Wrapper functions
    Public Declare Function FeliCaLib_Nfc_Initialize Lib "felica_nfc_library.dll" Alias "felicalib_nfc_initialize" () As Byte
    Public Declare Function FeliCaLib_Nfc_Uninitialize Lib "felica_nfc_library.dll" Alias "felicalib_nfc_uninitialize" () As Byte
    Public Declare Function FeliCaLib_Nfc_Open Lib "felica_nfc_library.dll" Alias "felicalib_nfc_open" (ByVal port_name As System.Text.StringBuilder) As Byte
    Public Declare Function FeliCaLib_Nfc_Close Lib "felica_nfc_library.dll" Alias "felicalib_nfc_close" () As Byte
    Public Declare Function FeliCaLib_Nfc_Start_Poll_Mode Lib "felica_nfc_library.dll" Alias "felicalib_nfc_start_poll_mode" (ByVal target_device As UInt32) As Byte
    Public Declare Function FeliCaLib_Nfc_Stop_Poll_Mode Lib "felica_nfc_library.dll" Alias "felicalib_nfc_stop_poll_mode" () As Byte
    Public Declare Function FeliCaLib_Nfc_Start_Dev_Access Lib "felica_nfc_library.dll" Alias "felicalib_nfc_start_dev_access" (ByVal target_device As UInt32) As Byte
    Public Declare Function FeliCaLib_Nfc_Stop_Dev_Access Lib "felica_nfc_library.dll" Alias "felicalib_nfc_stop_dev_access" (ByVal stop_mode As UInt32) As Byte
    Public Declare Function FeliCaLib_Nfc_Select_Device Lib "felica_nfc_library.dll" Alias "felicalib_nfc_select_device" () As Byte
    Public Declare Function FeliCaLib_Nfc_Deselect_Device Lib "felica_nfc_library.dll" Alias "felicalib_nfc_deselect_device" () As Byte
    Public Declare Function FeliCaLib_Nfc_Thru Lib "felica_nfc_library.dll" Alias "felicalib_nfc_thru" (ByVal command_packet_data As Byte(), ByVal command_packet_data_length As UInt16, ByVal reponse_packet_data As Byte(), ByRef reponse_packet_data_length As UInt16) As Byte
    Public Declare Function FeliCaLib_Nfc_Set_Timeout Lib "felica_nfc_library.dll" Alias "felicalib_nfc_set_timeout" (ByVal timeout As UInt32) As Byte
    Public Declare Function FeliCaLib_Nfc_Get_Timeout Lib "felica_nfc_library.dll" Alias "felicalib_nfc_get_timeout" (ByRef timeout As UInt32) As Byte
    Public Declare Function FeliCaLib_Nfc_Set_Poll_Callback_Parameters Lib "felica_nfc_library.dll" Alias "felicalib_nfc_set_poll_callback_parameters" (ByVal handle As IntPtr, ByVal msg_str_of_find As String, ByVal msg_str_of_enable As String) As Byte
    Public Declare Function FeliCaLib_Nfc_Set_Pnp_Callback_Parameters Lib "felica_nfc_library.dll" Alias "felicalib_nfc_set_pnp_callback_parameters" (ByVal handle As IntPtr, ByVal msg_str_of_find As String, ByVal msg_str_of_loss As String) As Byte
    Public Declare Function FeliCaLib_Nfc_Start_Plug_And_Play Lib "felica_nfc_library.dll" Alias "felicalib_nfc_start_plug_and_play" () As Byte
    Public Declare Function FeliCaLib_Nfc_Stop_Plug_And_Play Lib "felica_nfc_library.dll" Alias "felicalib_nfc_stop_plug_and_play" () As Byte
    Public Declare Function FeliCaLib_Nfc_Get_Last_Error Lib "felica_nfc_library.dll" Alias "felicalib_nfc_get_last_error" (ByVal error_info As UInt32()) As Byte
    Public Declare Function FeliCaLib_Nfc_Start_Logging Lib "felica_nfc_library.dll" Alias "felicalib_nfc_start_logging" (ByVal filename As String) As Byte
    Public Declare Function FeliCaLib_Nfc_Stop_Logging Lib "felica_nfc_library.dll" Alias "felicalib_nfc_stop_logging" () As Byte
End Class