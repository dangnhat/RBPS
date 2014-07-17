package dhbk.android.topomanager;

public class NodeInfo {
	private String name = "";
	private int nID = -1;
	private int pID = -1;
	private boolean isSche = false;
	private int valueSysBP = -1;
	private int valueDiasBP = -1;
	private int valueHR = -1;
	private int hour = -1;
	private int min = -1;
	private int date = -1;
	private int month = -1;
	private int year = -1;
	private String notice = "";
	
	public NodeInfo(int nodeID, int patientID, String name) {
		this.nID = nodeID;
		this.pID = patientID;
		this.name = name;
	}
	
	public void setIsSchedule(boolean isSchedule) {
		this.isSche = isSchedule;
	}
	
	public void setBpHrValue(int valueSysBp, int valueDiasBp, int valueHR) {
		this.valueSysBP = valueSysBp;
		this.valueDiasBP = valueDiasBp;
		this.valueHR = valueHR;
	}
	
	public void setPatientId(int pID) {
		this.pID = pID;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	public void setDateAndTimeStamp(int hour, int min, int date, int month, int year) {
		this.hour = hour;
		this.min = min;
		this.date = date;
		this.month = month;
		this.year = year;
	}
	
	public void setNotice(String notice) {
		this.notice = notice;
	}
	
	public int getNodeId() {
		return this.nID;
	}
	
	public int getPatientId() {
		return this.pID;
	}
	
	public String getName() {
		return this.name;
	}
	
	public boolean getIsSchedule() {
		return this.isSche;
	}
	
	public int getSysBpValue() {
		return this.valueSysBP;
	}
	
	public int getDiasBpValue() {
		return this.valueDiasBP;
	}
	
	public int getHrValue() {
		return this.valueHR;
	}
	
	public int getHour() {
		return this.hour;
	}
	
	public int getMinute() {
		return this.min;
	}
	
	public int getDate() {
		return this.date;
	}
	
	public int getMonth() {
		return this.month;
	}
	
	public int getYear() {
		return this.year;
	}
	
	public String getNotice() {
		return this.notice;
	}
	
}
