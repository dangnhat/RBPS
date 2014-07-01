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
		String htmlMeasure = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"Node ID: <font color=\"red\"><b>"+nID+"</b></font>.<br>"
				+"Patient's ID: <font color=\"red\"><b>"+pID+"</b></font>.<br>"
				+"Name: <font color=\"red\"><b>"+name+"</b></font></p>"
								
				+"<p><tt><b><font color=\"blue\"><u>Prediction data</u>:</font></b></tt><br>"
				+"Risk of prehypertension: <font color=\"green\">No</font><br>"
				
				+"Monthly avg BP (mmHg): <font color=\"red\">110</font>"
				+"<ul><li><img src=\"img/empty.png\"> Sys_BP > 135mmHg</img></li>"
				+"<li><img src=\"img/empty.png\"> Dias_BP > 85mmHg</img></li></ul>"
				
				+"Monthly avg HR (pulses/min): <font color=\"red\">70</font><br>"
				+"Monthly avg BMI (kg/m<sup>2</sup>): <font color=\"red\">21.5</font><br>"
				+"<img src=\"img/empty.png\"> Diabetes</img><br>"
				+"<img src=\"img/empty.png\"> Dyslipidemia</img><br>"
				+"<img src=\"img/empty.png\"> Atherosclerosis</img><br>"
				+"<img src=\"img/fill.png\"> Gout</img></p>";
		
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
