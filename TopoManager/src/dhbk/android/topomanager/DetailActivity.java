package dhbk.android.topomanager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;

public class DetailActivity extends Activity {
	Misc misc = new Misc();
	String[] specData;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_detail);
		
		WebView detail = (WebView)findViewById(R.id.detailWebView);
		detail.getSettings().setBuiltInZoomControls(true);
		
		/* Get extras from MainActivity */
		Intent intent = getIntent();
		Bundle bundle = intent.getExtras();
		String detailData = bundle.getString("data");
		
		/* Parse data from extra */
		specData = parseInfoFromDetailData(detailData);
		
		/* Common String */
		String emptyBox = "<img src=\"img/empty.png\">";
		String fillBox = "<img src=\"img/fill.png\">";
		String hrLine = "<hr size = \"3px\" noshade = \"noshade\">";
		String halfHrLine = "<hr width = \"50%\" align = \"center\" noshade = \"noshade\">";
		
		/* Get basic information */
		String nID = misc.parse(specData[0], "", "n");
		String pID = misc.parse(specData[0], "n", "p");
		String name = misc.parse(specData[0], "p", "");
		
		String[] dataSysBP = new String[7];
		String[] dataDiasBP = new String[7];
		String[] dataHR = new String[7];
		String[] dataTime = new String[5];
		String[] dataTimeStamp = new String[11];
		
		/* Get BP and HR information */
		for(int i = 0; i < 7; i++) {
			dataSysBP[i] = misc.parse(specData[i+1], "", "sys");
			dataDiasBP[i] = misc.parse(specData[i+1], "sys", "dias");
			dataHR[i] = misc.parse(specData[i+1], "dias", "hr");
		}
		
		/* Get most recently height */
		String recentlyHeight = misc.parse(specData[8], "", "cm");
		
		String weight = misc.parse(specData[9], "", "kg");
		
		/* Get timestamp */
		for(int i = 0; i < 4; i++) {
			dataTime[i] = misc.parse(specData[i*2+1], "hr", "");
		}
		dataTime[4] = misc.parse(specData[8], "cm", "");
		
		dataTimeStamp[0] = misc.setTextDateAndTime(dataTime[0]);
		dataTimeStamp[1] = misc.setTextDateAndTime(misc.parse(dataTime[1], 0, 12));
		dataTimeStamp[2] = misc.setTextDateAndTime(misc.parse(dataTime[1], 12, 24));
		dataTimeStamp[3] = misc.setTextDateAndTime(misc.parse(dataTime[1], 24));
		dataTimeStamp[4] = misc.setTextDateAndTime(misc.parse(dataTime[2], 0, 12));
		dataTimeStamp[5] = misc.setTextDateAndTime(misc.parse(dataTime[2], 12, 24));
		dataTimeStamp[6] = misc.setTextDateAndTime(misc.parse(dataTime[2], 24));
		dataTimeStamp[7] = misc.setTextDateAndTime(misc.parse(dataTime[3], 24));
		dataTimeStamp[8] = misc.setTextDateAndTime(misc.parse(dataTime[3], 12, 24));
		dataTimeStamp[9] = misc.setTextDateAndTime(misc.parse(dataTime[3], 24));
		dataTimeStamp[10] = misc.setTextDateAndTime(dataTime[4]);
		
		/* Get Medical history information */
		String[] history = new String[4];
		history[0] = misc.parse(specData[10], 0, 1);
		history[1] = misc.parse(specData[10], 1, 2);
		history[2] = misc.parse(specData[10], 2, 3);
		history[3] = misc.parse(specData[10], 3);
		for(int i = 0; i < 4; i++) {
			if(history[i].equals("0"))
				history[i] = emptyBox;
			else history[i] = fillBox;
		}
		
		String htmlDetail = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"_ Node ID: <b><font color=\"red\">"+nID+"</font></b>.<br>"
				+"_ Patient's ID: <b><font color=\"red\">"+pID+"</font></b>.<br>"
				+"_ Name: <b><font color=\"red\">"+name+"</font></b>."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>BP/HR data</u>:</font></b></tt><br>"
				+"_ Most recently BP/HR:<br>"
				+"<b><font color=\"red\">"+dataSysBP[0]+"</font>/<font color=\"blue\">"+dataDiasBP[0]+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"red\">"+dataHR[0]+" </font></b><small>(pulses/min)</small>, "
				+"<b><font color=\"red\">"+dataTimeStamp[0]+"</font></b>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily average BP/HR:<br>"
				+"<b><font color=\"red\">"+dataSysBP[1]+"</font>/<font color=\"blue\">"+dataDiasBP[1]+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"red\">"+dataHR[1]+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataSysBP[2]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[1]+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataDiasBP[2]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[2]+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"red\">"+dataHR[2]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[3]+"</font></b>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly average BP/HR:<br>"
				+"<b><font color=\"red\">"+dataSysBP[3]+"</font>/<font color=\"blue\">"+dataDiasBP[3]+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"red\">"+dataHR[3]+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataSysBP[4]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[4]+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataDiasBP[4]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[5]+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"red\">"+dataHR[4]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[6]+"</font></b>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly average BP/HR:<br>"
				+"<b><font color=\"red\">"+dataSysBP[5]+"</font>/<font color=\"blue\">"+dataDiasBP[5]+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"red\">"+dataHR[5]+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataSysBP[6]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[7]+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"red\">"+dataDiasBP[6]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[8]+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"red\">"+dataHR[6]+" </font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[9]+"</font></b>).</li></ul>"
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Height data</u>:</font></b></tt><br>"
				+"_ Most recently height <small>(cm)</small>: <b><font color=\"red\">"+recentlyHeight+"</font></b><br>"
				+ "(<b><font color=\"red\">"+dataTimeStamp[10]+"</font></b>)."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Weight data</u>:</font></b></tt><br>"
				+"_ Weekly weight <small>(kg)</small>: <b><font color=\"red\">"+weight+"</font></b>."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Medical history</u>:</font></b></tt><br>"
				+"&nbsp"+history[0]+" Diabetes?<br>"
				+"&nbsp"+history[1]+" Dyslipidemia?<br>"
				+"&nbsp"+history[2]+" Atherosclerosis?<br>"
				+"&nbsp"+history[3]+" Gout?"
				+"</p>";
		
		detail.loadDataWithBaseURL("file:///android_asset/", htmlDetail, "text/html", "UTF-8", null);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.detail, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	public String[] parseInfoFromDetailData(String data){
		String[] ret = new String[11];
		ret[0] = misc.parse(data, "", "recently");
		ret[1] = misc.parse(data, "recently", "avgdate");
		ret[2] = misc.parse(data, "avgdate", "peakdate");
		ret[3] = misc.parse(data, "peakdate", "avgweek");
		ret[4] = misc.parse(data, "avgweek", "peakweek");
		ret[5] = misc.parse(data, "peakweek", "avgmonth");
		ret[6] = misc.parse(data, "avgmonth", "peakmonth");
		ret[7] = misc.parse(data, "peakmonth", "height");
		ret[8] = misc.parse(data, "height", "weight");
		ret[9] = misc.parse(data, "weight", "history");
		ret[10] = misc.parse(data, "history", "");
		
		return ret;
	}
}
