package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
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
		
		String[] dataSysBP = new String[7];
		String[] dataDiasBP = new String[7];
		String[] dataHR = new String[7];
		String[] dataTimeStamp = new String[11];
		
		String name = "Pham Huu Dang Nhat";
		String nID = "1";
		String pID = "1";
		String emptyBox = "<img src=\"img/empty.png\">";
		String fillBox = "<img src=\"img/fill.png\">";
		String recentlyHeight = "170";
		String hrLine = "<hr size = \"3px\" noshade = \"noshade\">";
		String halfHrLine = "<hr width = \"50%\" align = \"center\" noshade = \"noshade\">";
		
		dataSysBP[0] = "110";
		dataSysBP[1] = "112";
		dataSysBP[2] = "118";
		dataSysBP[3] = "112";
		dataSysBP[4] = "118";
		dataSysBP[5] = "112";
		dataSysBP[6] = "118";
		
		dataDiasBP[0] = "66";
		dataDiasBP[1] = "68";
		dataDiasBP[2] = "69";
		dataDiasBP[3] = "68";
		dataDiasBP[4] = "69";
		dataDiasBP[5] = "69";
		dataDiasBP[6] = "69";
		
		dataHR[0] = "79";
		dataHR[1] = "75";
		dataHR[2] = "79";
		dataHR[3] = "75";
		dataHR[4] = "79";
		dataHR[5] = "75";
		dataHR[6] = "79";
		
		dataTimeStamp[0] = "14h30 29/06/2014";
		dataTimeStamp[1] = "14h30 29/06/2014";
		dataTimeStamp[2] = "14h30 29/06/2014";
		dataTimeStamp[3] = "14h30 29/06/2014";
		dataTimeStamp[4] = "14h30 29/06/2014";
		dataTimeStamp[5] = "14h30 29/06/2014";
		dataTimeStamp[6] = "14h30 29/06/2014";
		dataTimeStamp[7] = "14h30 29/06/2014";
		dataTimeStamp[8] = "14h30 29/06/2014";
		dataTimeStamp[9] = "14h30 29/06/2014";
		dataTimeStamp[10] = "14h30 29/06/2014";
		
		String htmlDetail = 
				"<p><tt><b><font color=\"blue\"><u>Basic info</u>:</font></b></tt><br>"
				+"_ Node ID: <font color=\"red\"><b>"+nID+"</b></font>.<br>"
				+"_ Patient's ID: <font color=\"red\"><b>"+pID+"</b></font>.<br>"
				+"_ Name: <font color=\"red\"><b>"+name+"</b></font>."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>BP/HR data</u>:</font></b></tt><br>"
				+"_ Most recently BP/HR:<br>"
				+"<font color=\"red\"><b>"+dataSysBP[0]+"/"+dataDiasBP[0]+" </b></font><small>(mmHg)</small>, "
				+"<font color=\"red\"><b>"+dataHR[0]+" </b></font><small>(pulses/min)</small>, "
				+"<font color=\"red\"><b>"+dataTimeStamp[0]+"</b></font>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily average BP/HR:<br>"
				+"<font color=\"red\"><b>"+dataSysBP[1]+"/"+dataDiasBP[1]+" </b></font><small>(mmHg)</small>, "
				+"<font color=\"red\"><b>"+dataHR[1]+" </b></font><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Daily peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataSysBP[2]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[1]+"</b></font>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataDiasBP[2]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[2]+"</b></font>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <font color=\"red\"><b>"+dataHR[2]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[3]+"</b></font>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly average BP/HR:<br>"
				+"<font color=\"red\"><b>"+dataSysBP[3]+"/"+dataDiasBP[3]+" </b></font><small>(mmHg)</small>, "
				+"<font color=\"red\"><b>"+dataHR[3]+" </b></font><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Weekly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataSysBP[4]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[4]+"</b></font>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataDiasBP[4]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[5]+"</b></font>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <font color=\"red\"><b>"+dataHR[4]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[6]+"</b></font>).</li></ul>"
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly average BP/HR:<br>"
				+"<font color=\"red\"><b>"+dataSysBP[5]+"/"+dataDiasBP[5]+" </b></font><small>(mmHg)</small>, "
				+"<font color=\"red\"><b>"+dataHR[5]+" </b></font><small>(pulses/min)</small>."
				+"</p>"
				
				+halfHrLine
				
				+"<p>_ Monthly peak BP/HR:"
				+"<ul><li>sys_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataSysBP[6]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[7]+"</b></font>).</li>"
				+"<li>dias_BP <small>(mmHg)</small>: <font color=\"red\"><b>"+dataDiasBP[6]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[8]+"</b></font>).</li>"
				+"<li>HR <small>(pulses/min)</small>: <font color=\"red\"><b>"+dataHR[6]+" </b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[9]+"</b></font>).</li></ul>"
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Height data</u>:</font></b></tt><br>"
				+"_ Most recently height <small>(cm)</small>: <font color=\"red\"><b>"+recentlyHeight+"</b></font><br>"
				+ "(<font color=\"red\"><b>"+dataTimeStamp[10]+"</b></font>)."
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Weight data</u>:</font></b></tt><br>"
				+"_ Weekly weight <small>(kg)</small>: "
				+"</p>"
				//===============================================================//
				+hrLine
				
				+"<p><tt><b><font color=\"blue\"><u>Medical history</u>:</font></b></tt><br>"
				+"&nbsp"+emptyBox+" Diabetes?<br>"
				+"&nbsp"+emptyBox+" Dyslipidemia?<br>"
				+"&nbsp"+emptyBox+" Atherosclerosis?<br>"
				+"&nbsp"+fillBox+" Gout?"
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
