package dhbk.android.topomanager;

import java.text.SimpleDateFormat;
import java.util.Calendar;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;

public class ScheduleActivity extends Activity {
	public final String newScheduleCmd = "11";
	public final String clearScheduleCmd = "12";
	CheckBox scheduleCheck;
	CheckBox absCheck;
	CheckBox relativeCheck;
	EditText hourIn;
	EditText minIn;
	EditText dateIn;
	EditText monthIn;
	EditText yearIn;
	LinearLayout bigLayout;
	LinearLayout dateLayout;
	Button applyBtn;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_schedule);
		scheduleCheck = (CheckBox)findViewById(R.id.scheduleCheckBox);
		absCheck = (CheckBox)findViewById(R.id.absCheckBox);
		relativeCheck = (CheckBox)findViewById(R.id.relativeCheckBox);
		bigLayout = (LinearLayout)findViewById(R.id.bigLayout);
		dateLayout = (LinearLayout)findViewById(R.id.dateLayout);
		hourIn = (EditText)findViewById(R.id.hourInput);
		minIn = (EditText)findViewById(R.id.minInput);
		dateIn = (EditText)findViewById(R.id.dateInput);
		monthIn = (EditText)findViewById(R.id.monthInput);
		yearIn = (EditText)findViewById(R.id.yearInput);
		applyBtn = (Button)findViewById(R.id.btnApply);

		SimpleDateFormat curFormater = new SimpleDateFormat("HHmmddMMyyyy"); 
		Calendar cal = Calendar.getInstance();
		String dateStr = curFormater.format(cal.getTime());
		
		String hourInstance = dateStr.substring(0, 2);
		String minInstance = dateStr.substring(2, 4);
		String dateInstance = dateStr.substring(4, 6);
		String monthInstance = dateStr.substring(6, 8);
		String yearInstance = dateStr.substring(8);
		
		hourIn.setText(hourInstance);
		minIn.setText(minInstance);
		dateIn.setText(dateInstance);
		monthIn.setText(monthInstance);
		yearIn.setText(yearInstance);
		
		scheduleCheck.setChecked(true);
		scheduleCheck.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(scheduleCheck.isChecked()) {
					bigLayout.setVisibility(View.VISIBLE);
				}else {
					bigLayout.setVisibility(View.GONE);
				}
			}
		});
		
		absCheck.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				relativeCheck.setChecked(false);
				absCheck.setChecked(true);
				dateLayout.setVisibility(View.VISIBLE);
			}
		});
		
		relativeCheck.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				relativeCheck.setChecked(true);
				absCheck.setChecked(false);
				dateLayout.setVisibility(View.GONE);
			}
		});
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
