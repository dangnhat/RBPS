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
		char[] arrDiasBpDec = misc.subCharArray(predictData, 8, 2);
		char[] arrDiasBpFrac = misc.subCharArray(predictData, 10, 2);
		char[] arrHrDec = misc.subCharArray(predictData, 12, 2);
		char[] arrHrFrac = misc.subCharArray(predictData, 14, 2);
		char[] arrBmiDec = misc.subCharArray(predictData, 16, 2);
		char[] arrBmiFrac = misc.subCharArray(predictData, 18, 2);
		char[] arrHistory = misc.subCharArray(predictData, 20, 4);
		
		int sysBpDec, sysBpFrac, diasBpDec, diasBpFrac, hrDec, hrFrac, bmiDec, bmiFrac;
		sysBpDec = misc.digitCharArray2Int(arrSysBpDec);
		sysBpFrac = misc.digitCharArray2Int(arrSysBpFrac);
		diasBpDec = misc.digitCharArray2Int(arrDiasBpDec);
		diasBpFrac = misc.digitCharArray2Int(arrDiasBpFrac);
		hrDec = misc.digitCharArray2Int(arrHrDec);
		hrFrac = misc.digitCharArray2Int(arrHrFrac);
		bmiDec = misc.digitCharArray2Int(arrBmiDec);
		bmiFrac = misc.digitCharArray2Int(arrBmiFrac);
		
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
		String monthlyAvgSysBp = String.valueOf(sysBpDec) + ".";
		if(sysBpFrac < 10)
			monthlyAvgSysBp += "0" + String.valueOf(sysBpFrac);
		else 
			monthlyAvgSysBp += String.valueOf(sysBpFrac);
		String monthlyAvgDiasBp = String.valueOf(diasBpDec) + ".";
		if(diasBpFrac < 10)
			monthlyAvgDiasBp += "0" + String.valueOf(diasBpFrac);
		else 
			monthlyAvgDiasBp += String.valueOf(diasBpFrac);
		
		String colorSysBp = "green";
		String colorDiasBp = "blue";
		String highSys = emptyBox;
		String highDias = emptyBox;
		if(sysBpDec >= 135) {
			highSys = fillBox;
			colorSysBp = "red";
		}
		if(diasBpDec >= 85) {
			highDias = fillBox;
			colorDiasBp = "red";
		}
		
		/* Get HR and BMI information */
		String monthlyAvgHr = String.valueOf(hrDec) + ".";
		if(hrFrac < 10)
			monthlyAvgHr += "0" + String.valueOf(hrFrac);
		else 
			monthlyAvgHr += String.valueOf(hrFrac);
		String monthlyAvgBmi = String.valueOf(bmiDec) + ".";
		if(bmiFrac < 10)
			monthlyAvgBmi += "0" + String.valueOf(bmiFrac);
		else 
			monthlyAvgBmi += String.valueOf(bmiFrac);
		
		/* Get Medical history information */
		String[] history = new String[4];
		for(int i = 0; i < 4; i++) {
			history[i] = "";
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
				+"_ Monthly average BMI <small>(kg/m<sup>2</sup>)</small>: <font color=\"blue\">"+monthlyAvgBmi+"</font>.</p>"
				
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
	
}
