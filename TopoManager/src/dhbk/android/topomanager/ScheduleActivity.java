package dhbk.android.topomanager;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class ScheduleActivity extends Activity {
	public final String newScheduleCmd = "11";
	public final String clearScheduleCmd = "12";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_schedule);

	}
	
	public void apllyTime(View viewClick) {
//		if(MainActivity.conn) {
//			/* Create a frame */
//            
//		}else
//			Toast.makeText(ScheduleActivity.this, "No connection. Please connect again!", Toast.LENGTH_SHORT).show();
		MainActivity.DATA = createDataFrame(newScheduleCmd, MainActivity.DATA);
		new SocketWorker(MainActivity.DATA, getApplicationContext()).execute();
		return;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.schedule, menu);
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
	
	/* Create a frame */
	public String createDataFrame(String cmd, String data) {
		String frame = "";
		int lengthData = data.length();
		frame += String.valueOf(lengthData) + cmd + data;
		
		return frame;
	}

}
