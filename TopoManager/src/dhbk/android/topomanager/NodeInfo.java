package dhbk.android.topomanager;

public class NodeInfo {
	private String name;
	private String nID;
	private String pID;
	private String valueSysBP;
	private String valueDiasBP;
	private String valueHR;
	private String timestamp;
	private String datestamp;
	private String otherNotice;
	private boolean timeOrOther;
	
	public NodeInfo(String nodeID, String patientID, String name) {
		this.nID = nodeID;
		this.pID = patientID;
		this.name = name;
	}
	
	public void setBpValue(String valueSysBp, String valueDiasBp) {
		this.valueSysBP = valueSysBp;
		this.valueDiasBP = valueDiasBp;
	}
	
	public void setHrValue(String valueHR) {
		this.valueHR = valueHR;
	}
	
	public void setDateAndTimeStampOrNotice(String timeOrNotice, boolean timeOrOther) {
		this.timeOrOther = timeOrOther;
		if(timeOrOther) {
			String hhmm = timeOrNotice.substring(0, 4);
			String ddmmyyyy = timeOrNotice.substring(4);
			this.timestamp = hhmm;
			this.datestamp = ddmmyyyy;
		}else {
			this.otherNotice = timeOrNotice;
		}
	}
	
	public String getNodeId() {
		return this.nID;
	}
	
	public String getPatientId() {
		return this.pID;
	}
	
	public String getName() {
		return this.name;
	}
	
	public String getSysBpValue() {
		return this.valueSysBP;
	}
	
	public String getDiasBpValue() {
		return this.valueDiasBP;
	}
	
	public String getHrValue() {
		return this.valueHR;
	}
	
	public String getTimeValue() {
		return this.timestamp;
	}
	
	public String getDateValue() {
		return this.datestamp;
	}
	
	public boolean getBoolTimeOrOther() {
		return this.timeOrOther;
	}
	
	public String getOtherNotice() {
		return this.otherNotice;
	}
	
	@Override
	public boolean equals(Object obj) {
		if(obj instanceof NodeInfo) {
			NodeInfo nInfo = (NodeInfo)obj;
			return this.pID.equalsIgnoreCase(nInfo.getPatientId());
		}
		return false;
	}
	
	@Override
	public int hashCode() {
		return this.pID.hashCode();
	}
	
}
