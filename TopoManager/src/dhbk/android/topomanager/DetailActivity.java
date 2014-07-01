package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
import android.text.Html;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;

public class DetailActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_detail);
		
		WebView detail = (WebView)findViewById(R.id.detailWebView);
		detail.getSettings().setBuiltInZoomControls(true);
		
		String name = "Pham Huu Dang Nhat";
		String nID = "1";
		String pID = "1";
		String htmlDetail = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"Node ID: <font color=\"red\"><b>"+nID+"</b></font>.<br>"
				+"Patient's ID: <font color=\"red\"><b>"+pID+"</b></font>.<br>"
				+"Name: <font color=\"red\"><b>"+name+"</b></font></p>"
				
				+"<p><tt><b><font color=\"blue\"><u>BP/HR data</u>:</font></b></tt><br>"
				+"Most recently BP/HR:<br>"
				+"<font color=\"red\"><b>110/66 </b></font>(mmHg), <font color=\"red\"><b>79 </b></font>(pulses/min), <font color=\"red\"><b>14h30 29/06/2014</b></font></p>"
				
				+"<p>Daily avg BP/HR:<br>"
				+"<font color=\"red\"><b>112/68 </b></font>(mmHg), <font color=\"red\"><b>75 </b></font>(pulses/min), <font color=\"red\"><b>14h30 29/06/2014</b></font></p>"
				
				+"<p>Daily peak BP/HR:"
				+"<ul><li>sys_BP (mmHg): <font color=\"red\"><b>118 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>dias_BP (mmHg): <font color=\"red\"><b>69 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>HR (pulses/min): <font color=\"red\"><b>79 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li></ul>"
				+"</p>"
				
				+"<p>Weekly avg BP/HR:<br>"
				+"<font color=\"red\"><b>112/68 </b></font>(mmHg), <font color=\"red\"><b>75 </b></font>(pulses/min)</p>"
				
				+"<p>Weekly peak BP/HR:"
				+"<ul><li>sys_BP (mmHg): <font color=\"red\"><b>118 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>dias_BP (mmHg): <font color=\"red\"><b>69 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>HR (pulses/min): <font color=\"red\"><b>79 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li></ul>"
				+"</p>"
				
				+"<p>Monthly avg BP/HR:<br>"
				+"<font color=\"red\"><b>112/68 </b></font>(mmHg), <font color=\"red\"><b>75 </b></font>(pulses/min)</p>"
				
				+"<p>Monthly peak BP/HR:"
				+"<ul><li>sys_BP (mmHg): <font color=\"red\"><b>118 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>dias_BP (mmHg): <font color=\"red\"><b>69 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li>"
				+"<li>HR (pulses/min): <font color=\"red\"><b>79 </b></font>, timestamp: <font color=\"red\"><b>14h30 29/06/2014</b></font></li></ul>"
				+"</p>"
				
				+"<p><tt><b><font color=\"blue\"><u>Height data (cm)</u>:</font></b></tt><br>"
				+"Most recently height: , Timestamp: </p>"
				
				+"<p><tt><b><font color=\"blue\"><u>Weight data (kg)</u>:</font></b></tt><br>"
				+"Weekly weight: , Timestamp: </p>"
				
				+"<p><tt><b><font color=\"blue\"><u>Medical history</u>:</font></b></tt><br>"
				+"<img src=\"img/empty.png\"> Diabetes</img><br>"
				+"<img src=\"img/empty.png\"> Dyslipidemia</img><br>"
				+"<img src=\"img/empty.png\"> Atherosclerosis</img><br>"
				+"<img src=\"img/fill.png\"> Gout</img></p>";
		
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
