package dhbk.android.topomanager;

import java.util.ArrayList;

import android.content.Context;
import android.text.Html;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class CustomListAdapter extends ArrayAdapter<NodeInfo> {
	Misc misc = new Misc();
	public ArrayList<NodeInfo> listNode;
	public int resource;
	public Context context;
	public NodeInfo nodeInfo;
	
	public TextView nIdView;
	public TextView pIdView;
	public TextView nameView;
	public TextView scheduleView;
	public TextView valueBpView;
	public TextView valueHrView;
	public TextView timestampView;
	public TextView noticeView;
	
	public CustomListAdapter(Context context, int textViewResourceId,
			ArrayList<NodeInfo> arrayNode) {
		super(context, textViewResourceId, arrayNode);

		this.context = context;
		this.resource = textViewResourceId;
		this.listNode = arrayNode;
	}
	
	public View getView(int position, View convertView, ViewGroup parent) {
		View nodeView = convertView;
		
		if(nodeView ==null ){
			nodeView = new CustomListView(getContext());
		}
		nodeInfo = listNode.get(position);
		if(nodeInfo != null){
			nIdView = ((CustomListView)nodeView).nIdView;
			pIdView = ((CustomListView)nodeView).pIdView;
			nameView = ((CustomListView)nodeView).nameView;
			scheduleView = ((CustomListView)nodeView).scheduleView;
			valueBpView = ((CustomListView)nodeView).valueBpView;
			valueHrView = ((CustomListView)nodeView).valueHrView;
			timestampView = ((CustomListView)nodeView).timestampView;
			noticeView = ((CustomListView)nodeView).noticeView;
			
			/* Display on view */
			nIdView.setText(String.valueOf(nodeInfo.getNodeId()));
			pIdView.setText(String.valueOf(nodeInfo.getPatientId()));
			nameView.setText(nodeInfo.getName());
			if(nodeInfo.getIsSchedule())
				scheduleView.setText("Scheduled");
			else scheduleView.setText("No schedule");
			
			String htmlValueBP = convertValueBP2Html(nodeInfo.getSysBpValue(), nodeInfo.getDiasBpValue());
			String htmlValueHR = convertValueHR2Html(nodeInfo.getHrValue());
			valueBpView.setText(Html.fromHtml(htmlValueBP));
			valueHrView.setText(Html.fromHtml(htmlValueHR));
			if(nodeInfo.getHour() < 0)
				timestampView.setText("");
			else
				timestampView.setText(misc.setTextDateAndTime(nodeInfo.getHour(), nodeInfo.getMinute(), 
											nodeInfo.getDate(), nodeInfo.getMonth(), nodeInfo.getYear()));
			noticeView.setText(nodeInfo.getNotice());
		}

		return nodeView;
	}
	
	public String convertValueBP2Html(int sysBP, int diasBP) {
		String sysBpStr = "===";
		String diasBpStr = "==";
		if(sysBP >= 0)
			sysBpStr = String.valueOf(sysBP);
		if(diasBP >= 0)
			diasBpStr = String.valueOf(diasBP);
		String result = "<u><b>BP</b></u>: <font color=\"red\">"+sysBpStr+"</font>/"
				+ "<font color=\"blue\">"+diasBpStr+"</font> (mmHg)";
		
		return result;
	}
	
	public String convertValueHR2Html(int HR) {
		String hrStr = "==";
		if(HR >= 0)
			hrStr = String.valueOf(HR);
		String result = "<u><b>HR</b></u>: <font color=\"blue\">"+hrStr+"</font> (ppm)";
		
		return result;
	}

}
