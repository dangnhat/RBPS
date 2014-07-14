package dhbk.android.topomanager;

import android.content.Context;
import android.view.LayoutInflater;
import android.widget.TableLayout;
import android.widget.TextView;

/* Get all properties of a node */
public class CustomListView extends TableLayout{
	Context context;
	TextView nIdView;
	TextView pIdView;
	TextView nameView;
	TextView scheduleView;
	TextView valueBpView;
	TextView valueHrView;
	TextView timestampView;
	TextView noticeView;
	
	public CustomListView(Context context) {
		super(context);
		
		this.context = context;
		LayoutInflater li = (LayoutInflater)this.getContext()
				 .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		li.inflate(R.layout.list_view, this, true);
		this.nIdView = (TextView)findViewById(R.id.nID);
		this.pIdView = (TextView)findViewById(R.id.pID);
		this.nameView = (TextView)findViewById(R.id.namePatient);
		this.scheduleView = (TextView)findViewById(R.id.scheduleText);
		this.valueBpView = (TextView)findViewById(R.id.valueBP);
		this.valueHrView = (TextView)findViewById(R.id.valueHR);
		this.timestampView = (TextView)findViewById(R.id.timestamp);
		this.noticeView = (TextView)findViewById(R.id.notice);
	}

}
