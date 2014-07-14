package dhbk.android.topomanager;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;

public class PredictActivity extends Activity {
	String[] specData;
	Misc misc = new Misc();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_predict);
		
		WebView predictView = (WebView)findViewById(R.id.predictWebView);
		predictView.getSettings().setBuiltInZoomControls(false);
		
		/* Common String */
		String emptyBox = "<img src=\"img/empty.png\">";
		String fillBox = "<img src=\"img/fill.png\">";
		String tab = "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
		String hrLine = "<hr size = \"3px\" noshade = \"noshade\">";
		
		/* Get extras from MainActivity */
		Intent intent = getIntent();
		Bundle bundle = intent.getExtras();
		String basic_info = bundle.getString("basic_info");
		char[] predictData = bundle.getCharArray("predict");
		
		
		/* Parse data from extra */
		char prehypertension_risk = predictData[3];
		char[] arrSysBpDec = misc.subCharArray(predictData, 4, 2);
		char[] arrSysBpFrac = misc.subCharArray(predictData, 6, 2);
		char[] arrDiasBp = misc.subCharArray(predictData, 8, 2); 
		char[] arrHr = misc.subCharArray(predictData, 12, 2);
		char[] arrBmi = misc.subCharArray(predictData, 16, 4); 
		char[] arrHistory = misc.subCharArray(predictData, 20, 4);
		
		/* Get basic information */
		String nID = misc.parse(basic_info, "", "n");
		String pID = misc.parse(basic_info, "n", "p");
		String name = misc.parse(basic_info, "p", "");
		
		/* Risk of prehypertension */
		String riskPrehyper, colorWarnRiskPrehyper;
		if(prehypertension_risk == 1) {
			riskPrehyper = "Yes";
			colorWarnRiskPrehyper = "red";
		}else {
			riskPrehyper = "No";
			colorWarnRiskPrehyper = "green";
		}
		
		/* Get BP information */
		String monthlyAvgSysBp = misc.parse(specData[2], "", "sys");
		int sysBP = Integer.parseInt(monthlyAvgSysBp);
		String monthlyAvgDiasBp = misc.parse(specData[2], "sys", "dias");
		int diasBP = Integer.parseInt(monthlyAvgDiasBp);
		
		String colorSysBp = "green";
		String colorDiasBp = "blue";
		String highSys = emptyBox;
		String highDias = emptyBox;
		if(sysBP > 135) {
			highSys = fillBox;
			colorSysBp = "red";
		}
		if(diasBP > 85) {
			highDias = fillBox;
			colorDiasBp = "red";
		}
		
		/* Get HR and BMI information */
		String monthlyAvgHr = misc.parse(specData[2], "dias", "hr");
		String monthlyAvgBmi = misc.parse(specData[2], "hr", "");
		
		/* Get Medical history information */
		String[] history = new String[4];
		history[0] = misc.parse(specData[3], 0, 1);
		history[1] = misc.parse(specData[3], 1, 2);
		history[2] = misc.parse(specData[3], 2, 3);
		history[3] = misc.parse(specData[3], 3);
		for(int i = 0; i < 4; i++) {
			if(arrHistory[i] == 0)
				history[i] = emptyBox;
			else history[i] = fillBox;
		}
		
		String htmlMeasure = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"_ Node ID: <font color=\"red\"><b>"+nID+"</b></font>.<br>"
				+"_ Patient's ID: <font color=\"red\"><b>"+pID+"</b></font>.<br>"
				+"_ Name: <font color=\"red\"><b>"+name+"</b></font>.</p>"
				
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Prediction data</u>:</font></b></tt><br>"
				+"_ Risk of prehypertension: <font color=\""+colorWarnRiskPrehyper+"\">"+riskPrehyper+"</font>.<br>"
				
				+"_ Monthly average BP <small>(mmHg)</small>: <font color=\""+colorSysBp+"\">"+monthlyAvgSysBp+"</font>/<font color=\""+colorDiasBp+"\">"+monthlyAvgDiasBp+"</font><br>"
				+tab+highSys+" Sys_BP > <b>135</b>mmHg?<br>"
				+tab+highDias+" Dias_BP > <b>85</b>mmHg?<br>"
				
				+"_ Monthly average HR <small>(pulses/min)</small>: <font color=\"green\">"+monthlyAvgHr+"</font>.<br>"
				+"_ Monthly average BMI <small>(kg/m<sup>2</sup>)</small>: <font color=\"green\">"+monthlyAvgBmi+"</font>.</p>"
				
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Medical history</u>:</font></b></tt><br>"
				+"&nbsp"+history[0]+" Diabetes?<br>"
				+"&nbsp"+history[1]+" Dyslipidemia?<br>"
				+"&nbsp"+history[2]+" Atherosclerosis?<br>"
				+"&nbsp"+history[3]+" Gout?</p>";
		
		predictView.loadDataWithBaseURL("file:///android_asset/", htmlMeasure, "text/html", "UTF-8", null);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.predict, menu);
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

	public String[] parseInfoFromPredictData(String data){
		String[] ret = new String[4];
		ret[0] = misc.parse(data, "", "prehyper");
		ret[1] = misc.parse(data, "prehyper", "avgmonth");
		ret[2] = misc.parse(data, "avgmonth", "history");
		ret[3] = misc.parse(data, "history", "");
		
		return ret;
	}
	
}
