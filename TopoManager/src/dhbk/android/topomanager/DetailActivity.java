package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
import android.text.Html;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

public class DetailActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_detail);
		TextView detail = (TextView)findViewById(R.id.detailView);
		String name = "Pham Huu Dang Nhat";
		String nID = "1";
		String pID = "1";
		String htmlDetail = 
				"<p><tt><u>Basic info</u>:</tt><br>"
				+"Node ID: <strong><font color=\"red\">"+nID+"</strong>.\t"
						+ "Patient's ID: <strong><font color=\"red\">"+pID+"</strong>.<br>"
				+"Name: <strong><font color=\"red\"><b>"+name+"</b></strong></p>"
				
				+"<p><tt><u>BP/HR data</u>:</tt><br>"
				+"Most recently BP/HR:<br>"
				+"</p>"
				+"<p>Daily avg BP/HR:<br>"
				+"</p>"
				+"<p>Daily peak BP/HR:<br>"
				+"</p>"
				+"<p>Weekly avg BP/HR:<br>"
				+"</p>"
				+"<p>Weekly peak BP/HR:<br>"
				+"</p>"
				+"<p>Monthly avg BP/HR:<br>"
				+"</p>"
				+"<p>Monthly peak BP/HR:<br>"
				+"</p>"
				
				+"<p><tt><u>Height data</u>:<tt><br>"
				+"</p>"
				
				+"<p><tt><u>Weight data</u>:<tt><br>"
				+"</p>"
				
				+"<p><tt><u>Medical history</u>:<tt><br>"
				+"</p>";
		detail.setText(Html.fromHtml(htmlDetail));
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
