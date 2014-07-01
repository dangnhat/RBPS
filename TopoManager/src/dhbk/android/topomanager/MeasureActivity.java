package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
import android.text.Html;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;
import android.widget.TextView;

public class MeasureActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_measure);
		
		WebView measureView = (WebView)findViewById(R.id.measureWebView);
		measureView.getSettings().setBuiltInZoomControls(true);
		
		String name = "Pham Huu Dang Nhat";
		String nID = "1";
		String pID = "1";
		String htmlMeasure = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"Node ID: <strong><font color=\"red\"><b>"+nID+"</b></font></strong>.<br>"
				+"Patient's ID: <strong><font color=\"red\"><b>"+pID+"</b></font></strong>.<br>"
				+"Name: <strong><font color=\"red\"><b>"+name+"</b></font></strong></p>"
				
				+"<p><tt><b><font color=\"blue\"><u>Measurement data</u>:</font></b></tt><br>"
				+"Sys_BP (mmHg): <b><font color=\"red\">110</b></font>.<br>"
				+"Dias_BP (mmHg): <b><font color=\"blue\">66</b></font>.<br>"
				+"HR (pulses/min): <b><font color=\"green\">79</b></font>.<br>"
				+"Timestamp:<br><font color=\"red\">17h23 28/06/2014</font></p>"
				
				+"<p><tt><b><font color=\"blue\"><u>Prediction data</u>:</font></b></tt><br>"
				+"Risk of prehypertension: <font color=\"green\">No</font><br>"
				+"Monthly avg BP (mmHg): <font color=\"red\">110</font><br>"
				+"Monthly avg HR (pulses/min): <font color=\"red\">70</font><br>"
				+"Monthly avg BMI (kg/m<sup>2</sup>): <font color=\"red\">21.5</font><br>"
				+"<img src=\"img/empty.png\"> Diabetes</img><br>"
				+"<img src=\"img/empty.png\"> Dyslipidemia</img><br>"
				+"<img src=\"img/empty.png\"> Atherosclerosis</img><br>"
				+"<img src=\"img/fill.png\"> Gout</img></p>";
		
		measureView.loadDataWithBaseURL("file:///android_asset/", htmlMeasure, "text/html", "UTF-8", null);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.measure, menu);
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
