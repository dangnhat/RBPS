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
	
	public String getHour(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(0, 2);
	}
	
	public String getMinute(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(2, 4);
	}
	
	public String getDate(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(4, 6);
	}
	
	public String getMonth(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(6, 8);
	}
	
	public String getYear(String hhmmddmmyyyy) {
		return hhmmddmmyyyy.substring(8);
	}
	
	public String setTextDateAndTime(String rawTime) {
		String result = "";
		
		String hour = getHour(rawTime);
		String min = getMinute(rawTime);
		String date = getDate(rawTime);
		String month = getMonth(rawTime);
		String year = getYear(rawTime);
		
		result = hour+"h"+min+", "+date+"/"+month+"/"+year; //e.g: 12h30, 25/05/2014
		return result;
	}
	
}
