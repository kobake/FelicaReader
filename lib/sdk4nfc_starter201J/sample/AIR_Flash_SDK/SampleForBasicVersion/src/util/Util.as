package util
{
	import mx.controls.CheckBox;
	import mx.controls.TextArea;
	import 	flash.utils.ByteArray;
	
	public class Util
	{
		public function Util()
		{
		}

		public static function changeData(chkbox:CheckBox, textArea:TextArea):void
		{
			if (textArea.length == 0)
			{
				return;
			}
			
			var temp:String;
			var data:ByteArray	= new ByteArray();
			if (chkbox.selected == true)
			{
				temp = textArea.text.replace(new RegExp("/\r\n|\r/*", "g"), "");		//	改行コードを削除
				
				data.writeBytes(Util.toArray(temp));
				textArea.text = data.toString();
			}
			else
			{
				data.writeUTFBytes(textArea.text);
				temp = "";
				for (var i:uint=0; i < data.length; i++)
				{
					temp += data[i].toString(16);
				}
				textArea.text = temp;
			}
		}
		
		public static function bytesToString(data:ByteArray):String
		{
			var result:String = "";
			var temp:String;
			
			for (var i:uint = 0; i < data.length; i++)
			{
				temp = data[i].toString(16);
				if (temp.length != 2)
				{
					temp = "0" + temp;
				}
				result += temp;
			}
			return	result;
		}
		
		protected static function toArray(hex:String):ByteArray
		{
			hex = hex.replace(/\s|:/gm,'');
			var a:ByteArray = new ByteArray;
			if (hex.length&1==1) hex="0"+hex;
			for (var i:uint=0;i<hex.length;i+=2) {
				a[i/2] = parseInt(hex.substr(i,2),16);
			}
			return a;
		}
		
		
		
	}
}
