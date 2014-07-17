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
		char[] detail_info = bundle.getCharArray("detail_info");
		char[] detail_bp = bundle.getCharArray("detail_bp");
		char[] detail_hr = bundle.getCharArray("detail_hr");
		char[] detail_height = bundle.getCharArray("detail_height");
		char[] detail_weight = bundle.getCharArray("detail_weight");
		char[] detail_history = bundle.getCharArray("detail_history");
		
		/* Common String */
		String emptyBox = "<img src=\"img/empty.png\">";
		String fillBox = "<img src=\"img/fill.png\">";
		String hrLine = "<hr size = \"3px\" noshade = \"noshade\">";
		String halfHrLine = "<hr width = \"50%\" align = \"center\" noshade = \"noshade\">";
		String tab = "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
		
		/* Get basic information */
		String nID = bundle.getString("nID");
		String pID = bundle.getString("pID");
		char[] dobArr = misc.subCharArray(detail_info, 3, 4);
			String dobStr = misc.timeCharArr2String(dobArr);
		char[] nameArr = misc.subCharArray(detail_info, 7, 20);
			String name = misc.concatNameArr2String(nameArr, 20);
		
		char[] mostRecentlyBp = misc.subCharArray(detail_bp, 4, 8);
		char[] mostRecentlyBpTime = misc.subCharArray(detail_bp, 12, 6);
		char[] bpInDay = misc.subCharArray(detail_bp, 18, 20);
		char[] bpInWeek = misc.subCharArray(detail_bp, 38, 28);
		char[] bpInMonth = misc.subCharArray(detail_bp, 66, 28);
		
			char[] mostRecentlySysBp = misc.subCharArray(mostRecentlyBp, 0, 4);
				String mostRecentSysBpStr = misc.floatCharArr2Str(mostRecentlySysBp);
			char[] mostRecentlyDiasBp = misc.subCharArray(mostRecentlyBp, 4, 4);
				String mostRecentDiasBpStr = misc.floatCharArr2Str(mostRecentlyDiasBp);
			
			char[] avgSysBpDay = misc.subCharArray(bpInDay, 0, 4);
				String avgSysBpDayStr = misc.floatCharArr2Str(avgSysBpDay);
			char[] avgDiasBpDay = misc.subCharArray(bpInDay, 4, 4);
				String avgDiasBpDayStr = misc.floatCharArr2Str(avgDiasBpDay);
			char[] peakSysBpDay = misc.subCharArray(bpInDay, 8, 4);
				String peakSysBpDayStr = misc.floatCharArr2Str(peakSysBpDay);
			char[] peakSysBpDayTime = misc.subCharArray(bpInDay, 12, 2);
				String peakSysBpDayTimeStr = misc.timeCharArr2String(peakSysBpDayTime);
			char[] peakDiasBpDay = misc.subCharArray(bpInDay, 14, 4);
				String peakDiasBpDayStr = misc.floatCharArr2Str(peakDiasBpDay);
			char[] peakDiasBpDayTime = misc.subCharArray(bpInDay, 18, 2);
				String peakDiasBpDayTimeStr = misc.timeCharArr2String(peakDiasBpDayTime);
			
			char[] avgSysBpWeek = misc.subCharArray(bpInWeek, 0, 4);
				String avgSysBpWeekStr = misc.floatCharArr2Str(avgSysBpWeek);
			char[] avgDiasBpWeek = misc.subCharArray(bpInWeek, 4, 4);
				String avgDiasBpWeekStr = misc.floatCharArr2Str(avgDiasBpWeek);
			char[] peakSysBpWeek = misc.subCharArray(bpInWeek, 8, 4);
				String peakSysBpWeekStr = misc.floatCharArr2Str(peakSysBpWeek);
			char[] peakSysBpWeekTime = misc.subCharArray(bpInWeek, 12, 6);
				String peakSysBpWeekTimeStr = misc.timeCharArr2String(peakSysBpWeekTime);
			char[] peakDiasBpWeek = misc.subCharArray(bpInWeek, 18, 4);
				String peakDiasBpWeekStr = misc.floatCharArr2Str(peakDiasBpWeek);
			char[] peakDiasBpWeekTime = misc.subCharArray(bpInWeek, 22, 6);
				String peakDiasBpWeekTimeStr = misc.timeCharArr2String(peakDiasBpWeekTime);
			
			char[] avgSysBpMonth = misc.subCharArray(bpInMonth, 0, 4);
				String avgSysBpMonthStr = misc.floatCharArr2Str(avgSysBpMonth);
			char[] avgDiasBpMonth = misc.subCharArray(bpInMonth, 4, 4);
				String avgDiasBpMonthStr = misc.floatCharArr2Str(avgDiasBpMonth);
			char[] peakSysBpMonth = misc.subCharArray(bpInMonth, 8, 4);
				String peakSysBpMonthStr = misc.floatCharArr2Str(peakSysBpMonth);
			char[] peakSysBpMonthTime = misc.subCharArray(bpInMonth, 12, 6);
				String peakSysBpMonthTimeStr = misc.timeCharArr2String(peakSysBpMonthTime);
			char[] peakDiasBpMonth = misc.subCharArray(bpInMonth, 18, 4);
				String peakDiasBpMonthStr = misc.floatCharArr2Str(peakDiasBpMonth);
			char[] peakDiasBpMonthTime = misc.subCharArray(bpInMonth, 22, 6);
				String peakDiasBpMonthTimeStr = misc.timeCharArr2String(peakDiasBpMonthTime);
		
		char[] mostRecentlyHr = misc.subCharArray(detail_hr, 4, 4);
			String mostRecentlyHrStr = misc.floatCharArr2Str(mostRecentlyHr);
		char[] mostRecentlyHrTime = misc.subCharArray(detail_hr, 8, 6);
			String mostRecentlyHrTimeStr = misc.timeCharArr2String(mostRecentlyHrTime);
		char[] hrInDay = misc.subCharArray(detail_hr, 14, 10);
		char[] hrInWeek = misc.subCharArray(detail_hr, 24, 14);
		char[] hrInMonth = misc.subCharArray(detail_hr, 38, 14);
		
			char[] avgHrInDay = misc.subCharArray(hrInDay, 0, 4);
				String avgHrInDayStr = misc.floatCharArr2Str(avgHrInDay);
			char[] peakHrInDay = misc.subCharArray(hrInDay, 4, 4);
				String peakHrInDayStr = misc.floatCharArr2Str(peakHrInDay);
			char[] peakHrInDayTime = misc.subCharArray(hrInDay, 8, 2);
				String peakHrInDayTimeStr = misc.timeCharArr2String(peakHrInDayTime);
			
			char[] avgHrInWeek = misc.subCharArray(hrInWeek, 0, 4);
				String avgHrInWeekStr = misc.floatCharArr2Str(avgHrInWeek);
			char[] peakHrInWeek = misc.subCharArray(hrInWeek, 4, 4);
				String peakHrInWeekStr = misc.floatCharArr2Str(peakHrInWeek);
			char[] peakHrInWeekTime = misc.subCharArray(hrInWeek, 8, 6);
				String peakHrInWeekTimeStr = misc.timeCharArr2String(peakHrInWeekTime);
			
			char[] avgHrInMonth = misc.subCharArray(hrInMonth, 0, 4);
				String avgHrInMonthStr = misc.floatCharArr2Str(avgHrInMonth);
			char[] peakHrInMonth = misc.subCharArray(hrInMonth, 4, 4);
				String peakHrInMonthStr = misc.floatCharArr2Str(peakHrInMonth);
			char[] peakHrInMonthTime = misc.subCharArray(hrInMonth, 8, 6);
				String peakHrInMonthTimeStr = misc.timeCharArr2String(peakHrInMonthTime);
		
		char mostRecentlyHeight = detail_height[3];
			String mostRecentlyHeightStr = String.valueOf((int)mostRecentlyHeight);
		char[] heightTimeStamp = misc.subCharArray(detail_height, 4, 4);
			String heightTimeStampStr = misc.timeCharArr2String(heightTimeStamp);
		
		int[] weight = new int[3];
		char[] weightTimeStamp_1, weightTimeStamp_2, weightTimeStamp_3;
		String[] weightTimeStampStr = {"", "", ""};
		weight[0] = (int)detail_weight[3];
		weightTimeStamp_1 = misc.subCharArray(detail_weight, 4, 4);
		weight[1] = (int)detail_weight[8];
		weightTimeStamp_2 = misc.subCharArray(detail_weight, 9, 4);
		weight[2] = (int)detail_weight[13];
		weightTimeStamp_3 = misc.subCharArray(detail_weight, 14, 4);
		String[] weightStr = {"", "", ""};
		if(weight[0] > 0) {
			weightStr[0] = String.valueOf(weight[0]);
			weightTimeStampStr[0] = misc.timeCharArr2String(weightTimeStamp_1);
		}
		if(weight[1] > 0) {
			weightStr[1] = String.valueOf(weight[1]);
			weightTimeStampStr[1] = misc.timeCharArr2String(weightTimeStamp_2);
		}
		if(weight[2] > 0) {
			weightStr[2] = String.valueOf(weight[2]);
			weightTimeStampStr[2] = misc.timeCharArr2String(weightTimeStamp_3);
		}
		
		/* Get Medical history information */
		String[] history = new String[4];
		for(int i = 0; i < 4; i++) {
			if(detail_history[i+3] == 0)
				history[i] = emptyBox;
			else history[i] = fillBox;
		}
		
		String htmlDetail = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"_ Node ID: <b><font color=\"red\">"+nID+"</font></b>.<br>"
				+"_ Patient's ID: <b><font color=\"red\">"+pID+"</font></b>.<br>"
				+"_ Name: <b><font color=\"red\">"+name+"</font></b>.<br>"
				+"_ Date of birth: <b><font color=\"red\">"+dobStr+"</font></b>."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>BP/HR data</u>:</font></b></tt><br>"
				+"_ Most recently BP/HR:<br>"
				+"<b><font color=\"red\">"+mostRecentSysBpStr+"</font>/<font color=\"blue\">"+mostRecentDiasBpStr+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"green\">"+mostRecentlyHrStr+" </font></b><small>(pulses/min)</small><br>"
				+ "(<b><font color=\"red\">"+mostRecentlyHrTimeStr+"</font></b>)."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily average BP/HR:<br>"
				+"<b><font color=\"red\">"+avgSysBpDayStr+"</font>/<font color=\"blue\">"+avgDiasBpDayStr+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"green\">"+avgHrInDayStr+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+peakSysBpDayStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakSysBpDayTimeStr+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"blue\">"+peakDiasBpDayStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakDiasBpDayTimeStr+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"green\">"+peakHrInDayStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakHrInDayTimeStr+"</font></b>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly average BP/HR:<br>"
				+"<b><font color=\"red\">"+avgSysBpWeekStr+"</font>/<font color=\"blue\">"+avgDiasBpWeekStr+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"green\">"+avgHrInWeekStr+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+peakSysBpWeekStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakSysBpWeekTimeStr+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"blue\">"+peakDiasBpWeekStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakDiasBpWeekTimeStr+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"green\">"+peakHrInWeekStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakHrInWeekTimeStr+"</font></b>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly average BP/HR:<br>"
				+"<b><font color=\"red\">"+avgSysBpMonthStr+"</font>/<font color=\"blue\">"+avgDiasBpMonthStr+" </font></b><small>(mmHg)</small>, "
				+"<b><font color=\"green\">"+avgHrInMonthStr+" </font></b><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <b><font color=\"red\">"+peakSysBpMonthStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakSysBpMonthTimeStr+"</font></b>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <b><font color=\"blue\">"+peakDiasBpMonthStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakDiasBpMonthTimeStr+"</font></b>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <b><font color=\"green\">"+peakHrInMonthStr+" </font></b><br>"
				+ "(<b><font color=\"red\">"+peakHrInMonthTimeStr+"</font></b>).</li></ul>"
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Height data</u>:</font></b></tt><br>"
				+"_ Most recently height <small>(cm)</small>: <b><font color=\"green\">"+mostRecentlyHeightStr+"</font></b><br>"
				+ "(<b><font color=\"red\">"+heightTimeStampStr+"</font></b>)."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Weight data</u>:</font></b></tt><br>"
				+"_ Weekly weight <small>(kg)</small>: <br>"
				+tab+" 1. <b><font color=\"green\">"+weightStr[0]+"</font></b><br>"
				+tab+tab+"(<b><font color=\"red\">"+weightTimeStampStr[0]+"</font></b>).<br>"
				+tab+" 2. <b><font color=\"green\">"+weightStr[1]+"</font></b><br>"
				+tab+tab+"(<b><font color=\"red\">"+weightTimeStampStr[1]+"</font></b>).<br>"
				+tab+" 3. <b><font color=\"green\">"+weightStr[2]+"</font></b><br>"
				+tab+tab+"(<b><font color=\"red\">"+weightTimeStampStr[2]+"</font></b>)."
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
	
}
