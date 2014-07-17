package dhbk.android.topomanager;

public class Misc {
	public String parse(String data, String firstString, String lastString) {
		String parsedData = "";
		int offset = 0;
		int startIndexReturnData = 0;
		int endIndexReturnData = 0;
		
		if(!firstString.equals("")) {
			offset = firstString.length();
			startIndexReturnData = data.indexOf(firstString);
			if(startIndexReturnData < 0)
				return parsedData;
			else
				startIndexReturnData += offset;
		}
		
		if(lastString.equals("")) {
			parsedData = data.substring(startIndexReturnData);
		}else {
			endIndexReturnData = data.indexOf(lastString);
			if(endIndexReturnData >= 0)
				parsedData = data.substring(startIndexReturnData, endIndexReturnData);	
		}
		
		return parsedData;
	}
	
	public String parse(String data, int start, int end) {
		String parsedData = "";
		parsedData = data.substring(start, end);
		
		return parsedData;
	}
	
	public String parse(String data, int start) {
		String parsedData = "";
		parsedData = data.substring(start);
		
		return parsedData;
	}
	
	public String getHourFromString(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(0, 2);
	}
	
	public String getMinuteFromString(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(2, 4);
	}
	
	public String getDateFromString(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(4, 6);
	}
	
	public String getMonthFromString(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(6, 8);
	}
	
	public String getYearFromString(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(8);
	}
	
	public String setTextDateAndTime(String rawTime) {
		String result = "";
		
		String hour = getHourFromString(rawTime);
		String min = getMinuteFromString(rawTime);
		String date = getDateFromString(rawTime);
		String month = getMonthFromString(rawTime);
		String year = getYearFromString(rawTime);
		
		result = hour+"h"+min+", "+date+"/"+month+"/"+year; //e.g: 12h30, 25/05/2014
		return result;
	}
	
	public String setTextDateAndTime(int hh, int mm, int dd, int MM, int yyyy) {
		String result = "";
		String hour, min, date, month, year;
		if(hh < 10)
			hour = "0"+String.valueOf(hh);
		else hour = String.valueOf(hh);
		if(mm < 10)
			min = "0"+String.valueOf(mm);
		else min = String.valueOf(mm);
		if(dd < 10)
			date = "0"+String.valueOf(dd);
		else date = String.valueOf(dd);
		if(MM < 10)
			month = "0"+String.valueOf(MM);
		else month = String.valueOf(MM);
		year = String.valueOf(yyyy);
		
		result = hour+"h"+min+", "+date+"/"+month+"/"+year; //e.g: 12h30, 25/05/2014
		return result;
	}
	
	public char[] subCharArray(char[] source, int start, int numOfChar) {
		char[] result = new char[numOfChar];
		for(int i = 0; i < numOfChar; i++) {
			result[i] = source[start+i];
		}
		return result;
	}
	
	public boolean insData2Arr(char[] des, char[] source, int start) {
		int lengDes = des.length;
		int lengSubArr = source.length;
		if((lengDes - start) < lengSubArr) {
			return false;
		}else {
			for(int i = start; i < lengSubArr+start; i++) {
				des[i] = source[i-start];
			}
			return true;
		}
	}
	
	/* Input: digit char array has 2-4 elements */
	public int digitCharArray2Int(char[] source) {
		int result = -1;
		int leng = source.length;
		int firstByte = (int)source[0];
		int secondByte = (int)source[1];
		result = firstByte|(secondByte<<8);
		int thirdByte;
		if(leng == 3) {
			thirdByte = (int)source[2];
			result = result|(thirdByte<<16);
		}
		if(leng == 4) {
			thirdByte = (int)source[2];
			int lastByte = (int)source[3];
			result = result|(thirdByte<<16)|(lastByte<<24);
		}
		return result;
	}
	
	public String digitCharArray2String(char[] source) {
		String result = "";
		int leng = source.length;
		for(int i = 0; i < leng; i++) {
			result += String.valueOf((int)source[i])+" ";
		}
		return result;
	}
	
	public char[] int2charArray(int source) {
		char[] result = new char[4];
		for(int i = 0; i < 4; i++) {
			result[i] = (char)((source>>(i*8))&0xff);
		}
		return result;
	}
	
	/* change 4elements char array of float number to String */
	public String floatCharArr2Str(char[] source) {
		String result = "";
		char[] decByte = subCharArray(source, 0, 2);
		int dec = digitCharArray2Int(decByte);
		char[] fracByte = subCharArray(source, 2, 2);
		int frac = digitCharArray2Int(fracByte);
		result = String.valueOf(dec)+"."+String.valueOf(frac);
		return result;
	}
	
	public String timeCharArr2String(char[] time) {
		String result = "";
		/* hh:mm */
		if(time.length == 2) {
			int hour = (int)time[0];
			int min = (int)time[1];
			String hh = String.valueOf(hour);
			String mm = String.valueOf(min);
			if(hour < 10) 
				hh = "0"+String.valueOf(hour);
			if(min < 10)
				mm = "0"+String.valueOf(min);
			result = hh+"h"+mm;
		}
		/* dd/MM/yyyy */
		if(time.length == 4) {
			int date = (int)time[0];
			int month = (int)time[1];
			char[] yearArr = subCharArray(time, 2, 2);
			int year = digitCharArray2Int(yearArr);
			
			String dd = String.valueOf(date);
			String MM = String.valueOf(month);
			if(date < 10) 
				dd = "0"+String.valueOf(date);
			if(month < 10)
				MM = "0"+String.valueOf(month);
			result = dd+"/"+MM+"/"+String.valueOf(year);
		}
		/* hh:mm, dd/MM/yyyy */
		if(time.length == 6) {
			int hour = (int)time[0];
			int min = (int)time[1];
			String hh = String.valueOf(hour);
			String mm = String.valueOf(min);
			if(hour < 10) 
				hh = "0"+String.valueOf(hour);
			if(min < 10)
				mm = "0"+String.valueOf(min);
			result = hh+"h"+mm;
			
			int date = (int)time[2];
			int month = (int)time[3];
			char[] yearArr = subCharArray(time, 4, 2);
			int year = digitCharArray2Int(yearArr);
			
			String dd = String.valueOf(date);
			String MM = String.valueOf(month);
			if(date < 10) 
				dd = "0"+String.valueOf(date);
			if(month < 10)
				MM = "0"+String.valueOf(month);
			result += ", "+dd+"/"+MM+"/"+String.valueOf(year);
		}
		
		return result;
	}
	
	public String concatNameArr2String(char[] source, int length) {
		String result = "";
		char[] newNameArr;
		int i = 0;
		for(i = 0; i < length; i++) {
			if(source[i] == 0) {
				break;
			}
		}
		newNameArr = subCharArray(source, 0, i);
		result = String.valueOf(newNameArr);
		return result;
	}
	
	/* Create a frame */
	public char[] createDataFrame(int cmd, char[] data) {
		char[] frame = new char[258];
		frame[0] = (char)data.length;
		char[] command = int2charArray(cmd);
		frame[1] = command[0];
		frame[2] = command[1];
		insData2Arr(frame, data, 3);
		return frame;
	}
	
}
