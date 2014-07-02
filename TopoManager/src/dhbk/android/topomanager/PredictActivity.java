package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;

public class PredictActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_predict);
		
		WebView predictView = (WebView)findViewById(R.id.predictWebView);
		predictView.getSettings().setBuiltInZoomControls(true);
		
		String name = "Pham Huu Dang Nhat";
		String nID = "1";
		String pID = "1";
		String emptyBox = "<img src=\"img/empty.png\">";
		String fillBox = "<img src=\"img/fill.png\">";
		String tab = "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
		String hrLine = "<hr size = \"3px\" noshade = \"noshade\">";
		
		String monthlyAvgSysBp = "110";
		String monthlyAvgDiasBp = "66";
		String monthlyAvgHr = "70";
		String monthlyAvgBmi = "21.5";
		String riskPrehyper = "No";
		
		String htmlMeasure = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"_ Node ID: <font color=\"red\"><b>"+nID+"</b></font>.<br>"
				+"_ Patient's ID: <font color=\"red\"><b>"+pID+"</b></font>.<br>"
				+"_ Name: <font color=\"red\"><b>"+name+"</b></font>.</p>"
				
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Prediction data</u>:</font></b></tt><br>"
				+"_ Risk of prehypertension: <font color=\"green\">"+riskPrehyper+"</font>.<br>"
				
				+"_ Monthly average BP <small>(mmHg)</small>: <font color=\"green\">"+monthlyAvgSysBp+"/"+monthlyAvgDiasBp+"</font><br>"
				+tab+emptyBox+" Sys_BP > <b>135</b>mmHg?<br>"
				+tab+emptyBox+" Dias_BP > <b>85</b>mmHg?<br>"
				
				+"_ Monthly average HR <small>(pulses/min)</small>: <font color=\"green\">"+monthlyAvgHr+"</font>.<br>"
				+"_ Monthly average BMI <small>(kg/m<sup>2</sup>)</small>: <font color=\"green\">"+monthlyAvgBmi+"</font>.</p>"
				
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Medical history</u>:</font></b></tt><br>"
				+"&nbsp"+emptyBox+" Diabetes?<br>"
				+"&nbsp"+emptyBox+" Dyslipidemia?<br>"
				+"&nbsp"+emptyBox+" Atherosclerosis?<br>"
				+"&nbsp"+fillBox+" Gout?</p>";
		
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
