/*
 * nfc_sample_01.cs
 * Copyright 2009,2011 Sony Corporation
 */
using System;
using System.Threading;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;

namespace nfc_sample_01
{
	class nfc_sample_01
	{
		private const Int32 EXIT_FAILED = 1;
		private const Int32 EXIT_SUCCEED = 0;
		private const Int32 BUFSIZ = 512;

		private static felica_nfc_dll_wrapper FeliCaNfcDllWrapperClass =
			new felica_nfc_dll_wrapper();

		[STAThread]
		static Int32 Main(string[] args)
		{
			bool bRet = false;
			String msg_str_of_find = "find";
			String msg_str_of_enable = "enable";
			ListenerWindow lw = new ListenerWindow();

            UInt32 card_find_message = RegisterWindowMessage(msg_str_of_find);
			if (card_find_message == 0) 
			{
				Console.Write("Failed: RegisterWindowMessage\n");
				return EXIT_FAILED;
			}

            UInt32 card_enable_message = RegisterWindowMessage(msg_str_of_enable);
			if (card_enable_message == 0) 
			{
				Console.Write("Failed: RegisterWindowMessage\n");
				return EXIT_FAILED;
			}

			bRet = lw.WatchMessage(card_find_message);
			if (bRet == false) 
			{
				Console.Write("Failed: WatchMessage\n");
				return EXIT_FAILED;
			}

			bRet = lw.WatchMessage(card_enable_message);
			if (bRet == false) 
			{
				Console.Write("Failed: WatchMessage\n");
				return EXIT_FAILED;
			}

			MessageHandler messageHandler
				= new MessageHandler(card_find_message, card_enable_message);
			lw.handler
				+= new MessageReceivedEventHandler(messageHandler.messageHandlerFunc);

			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcInitialize();
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcInitialize\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			StringBuilder port_name = new StringBuilder("USB0");
			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcOpen(port_name);
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcOpen\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcSetPollCallbackParameters(
				lw.Handle,
				msg_str_of_find,
				msg_str_of_enable);
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcSetPollCallbackParameters\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			const Int32 DEVICE_TYPE_NFC_14443A_18092_106K = 0x00000001;
            UInt32 target_device = DEVICE_TYPE_NFC_14443A_18092_106K;
			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcStartPollMode(target_device);
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcStartPollMode\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			Application.Run(lw);

			if (messageHandler.bRet == false) 
			{
				ErrorRoutine();
				return EXIT_FAILED;
			}

            byte[] command_packet_data = new byte[2] { 0x30, 0x00 };	// Command for Mifare ultralight, &H30: 16byte Reading command, &H0: Start address
            UInt16 command_packet_data_length = 2;
            byte[] response_packet_data = new byte[BUFSIZ];
            UInt16 response_packet_data_length = 0x00;
			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcThru(
				command_packet_data,
				command_packet_data_length,
				response_packet_data,
				ref response_packet_data_length);
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcThru\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			for (Int32 idx = 0; idx < 16; idx++)
			{
				Console.Write("response_packet_data[{0}]: 0x{1:X2}\n", idx, response_packet_data[idx]);
			}

            UInt32 RE_NOTIFICATION_SAME_DEVICE = 0x00;
            UInt32 stop_mode = RE_NOTIFICATION_SAME_DEVICE;
			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcStopDevAccess(stop_mode);
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcStopDevAccess\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}
			
			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcStopPollMode();
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcStopPollMode\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcClose();
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcClose\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcUninitialize();
			if (bRet == false)
			{
				Console.Write("Failed: FeliCaLibNfcUninitialize\n");
				ErrorRoutine();
				return EXIT_FAILED;
			}

			Console.Write("Succeed\n");
			
			return EXIT_SUCCEED;
		}

		[DllImport("User32.dll")]
        extern static UInt32 RegisterWindowMessage(String lpString);

		static void ErrorRoutine()
		{
            UInt32[] error_info = new UInt32[2] { 0, 0 };
			FeliCaNfcDllWrapperClass.FeliCaLibNfcGetLastError(error_info);
			Console.Write("error_info[0]: 0x{0:X8}\nerror_info[1]: 0x{1:X8}\n", error_info[0], error_info[1]);

			FeliCaNfcDllWrapperClass.FeliCaLibNfcClose();
			FeliCaNfcDllWrapperClass.FeliCaLibNfcUninitialize();
			return;
		}
	}

	public delegate void MessageReceivedEventHandler(object sender, MessageReceivedEventArgs e);

	public class MessageReceivedEventArgs : EventArgs
	{
		private readonly Message _message;
		public MessageReceivedEventArgs(Message message) { _message = message; }
		public Message Message { get { return _message; } }
	}

	public class ListenerWindow : Form
	{
		private const Int32 MAX_MESSAGES = 2;
		public event MessageReceivedEventHandler handler;
        private UInt32[] messageSet = new UInt32[MAX_MESSAGES];
		private Int32 registeredMessage = 0;

        public bool WatchMessage(UInt32 message)
		{
			if (registeredMessage < messageSet.Length) 
			{
				messageSet[registeredMessage] = message;
				registeredMessage++;
				return true;
			} 
			else 
			{
				return false;
			}
		}
		protected override CreateParams CreateParams
		{
			get
			{
				const Int32 WS_EX_TOOLWINDOW = 0x80;
				const Int64 WS_POPUP = 0x80000000;
				const Int32 WS_VISIBLE = 0x10000000;
				const Int32 WS_SYSMENU = 0x80000;
				const Int32 WS_MAXIMIZEBOX = 0x10000;

				CreateParams cp = base.CreateParams;
				cp.ExStyle = WS_EX_TOOLWINDOW;
				cp.Style = unchecked((Int32)WS_POPUP) |
					WS_VISIBLE | WS_SYSMENU | WS_MAXIMIZEBOX;
				cp.Width = 0;
				cp.Height = 0;

				return cp;
			}
		}
		protected override void WndProc(ref Message m)
		{
			bool handleMessage = false;
			for (Int32 i = 0; i < registeredMessage; i++)
			{
				if (messageSet[i] == m.Msg) 
				{
					handleMessage = true;
				}
			}

			if (handleMessage && handler != null)
			{
				handler(null, new MessageReceivedEventArgs(m));
			}
			base.WndProc(ref m);
			return;
		}
	}
	
	public class MessageHandler
	{
		public bool bRet;
        private UInt32 target_number;
        private UInt32 card_find_message;
        private UInt32 card_enable_message;
		private static felica_nfc_dll_wrapper FeliCaNfcDllWrapperClass =
			new felica_nfc_dll_wrapper();

		public MessageHandler(
            UInt32 findMsg,
            UInt32 enableMsg)
		{
			card_find_message = findMsg;
			card_enable_message = enableMsg;
		}

		public void messageHandlerFunc(object sender, MessageReceivedEventArgs e)
		{
			bRet = false;
			if (e.Message.Msg == card_find_message)
			{
				IntPtr pDevInfo = e.Message.LParam;
				IntPtr pDeviceData_A;
				if (IntPtr.Size == 8) 
				{
					pDeviceData_A = (IntPtr)((Int64)pDevInfo
						+ (Int64)Marshal.OffsetOf(typeof(DEVICE_INFO), "dev_info"));
				} 
				else 
				{
					pDeviceData_A = (IntPtr)((Int32)pDevInfo
						+ (Int32)Marshal.OffsetOf(typeof(DEVICE_INFO), "dev_info"));
				}

				DEVICE_DATA_NFC_14443A_18092_106K DeviceData_A =
					(DEVICE_DATA_NFC_14443A_18092_106K)
					Marshal.PtrToStructure(pDeviceData_A,
					typeof(DEVICE_DATA_NFC_14443A_18092_106K));

				target_number = DeviceData_A.target_number;
				bRet = FeliCaNfcDllWrapperClass.FeliCaLibNfcStartDevAccess(target_number);
				if (bRet == false)
				{
					Console.Write("Failed: FeliCaLibNfcStartDevAccess\n");
					Application.Exit();
					return;
				}
			}
			else if (e.Message.Msg == card_enable_message)
			{
				bRet = true;
				Application.Exit();
				return;
			}
			return;
		}
	}
}
