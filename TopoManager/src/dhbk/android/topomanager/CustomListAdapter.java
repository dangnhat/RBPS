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
			
			/* Display on view */
			nIdView.setText(nodeInfo.getNodeId());
			pIdView.setText(nodeInfo.getPatientId());
			nameView.setText(nodeInfo.getName());
			scheduleView.setText(nodeInfo.getIsSchedule());
			
			String htmlValueBP = convertValueBP2Html(nodeInfo.getSysBpValue(), nodeInfo.getDiasBpValue());
			String htmlValueHR = convertValueHR2Html(nodeInfo.getHrValue());
			valueBpView.setText(Html.fromHtml(htmlValueBP));
			valueHrView.setText(Html.fromHtml(htmlValueHR));
			timestampView.setText(setTextDateAndTimeOrNotice());
		}

		return nodeView;
	}
	
	public String convertValueBP2Html(String sysBP, String diasBP) {
		String result = "<u><b>BP</b></u>: <font color=\"red\">"+sysBP+"</font>/"
				+ "<font color=\"blue\">"+diasBP+"</font> (mmHg)";
		
		return result;
	}
	
	public String convertValueHR2Html(String HR) {
		String result = "<u><b>HR</b></u>: <font color=\"blue\">"+HR+"</font> (ppm)";
		
		return result;
	}
	
	public String setTextDateAndTimeOrNotice() {
		String result = "";
		if(nodeInfo.getBoolTimeOrOther()) {
			String hhmm = nodeInfo.getTimeValue();
			String ddmmyyyy = nodeInfo.getDateValue();
			result = misc.setTextDateAndTime(hhmm+ddmmyyyy);
		}else {
			result = nodeInfo.getOtherNotice();
		}
		return result;
	}

}
