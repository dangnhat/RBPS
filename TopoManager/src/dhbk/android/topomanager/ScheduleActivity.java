package dhbk.android.topomanager;

import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

public class ScheduleActivity extends Activity {
	Misc misc = new Misc();
	final int SERVER_PORT = 9999;
	final String SERVER_IP_DEFAULT = "192.168.150.1";
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
		
		Intent intent = getIntent();
		Bundle bundle = intent.getExtras();
		String scheduleInfo = bundle.getString("data");
		
		String isSchedule = misc.parse(scheduleInfo, "", "schedule");
		String isAbs = "0";
		String timeScheduled = "";
		String hour, min, date, month, year;
		if(isSchedule.equals("1")) {
			bigLayout.setVisibility(View.VISIBLE);
			scheduleCheck.setChecked(true);
			timeScheduled = misc.parse(scheduleInfo, "abs", "");
			hour = misc.getHour(timeScheduled);
			min = misc.getMinute(timeScheduled);
			hourIn.setText(hour);
			minIn.setText(min);
			isAbs = misc.parse(scheduleInfo, "schedule", "abs");
			if(isAbs.equals("1")) {
				absCheck.setChecked(true);
				relativeCheck.setChecked(false);
				dateLayout.setVisibility(View.VISIBLE);
				date = misc.getDate(timeScheduled);
				month = misc.getMonth(timeScheduled);
				year = misc.getYear(timeScheduled);
				dateIn.setText(date);
				monthIn.setText(month);
				yearIn.setText(year);
			}else if(isAbs.equals("0")) {
				absCheck.setChecked(false);
				relativeCheck.setChecked(true);
				dateLayout.setVisibility(View.INVISIBLE);
			}
		}else {
			scheduleCheck.setChecked(false);
			bigLayout.setVisibility(View.INVISIBLE);
		}
		
		scheduleCheck.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(scheduleCheck.isChecked()) {
					bigLayout.setVisibility(View.VISIBLE);
				}else {
					bigLayout.setVisibility(View.INVISIBLE);
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
				dateLayout.setVisibility(View.INVISIBLE);
			}
		});
	}
	
	public void apllyTime(View viewClick) {
		SimpleDateFormat curFormater = new SimpleDateFormat("HHmmddMMyyyy"); 
		Calendar cal = Calendar.getInstance();
		String timeInstance = curFormater.format(cal.getTime());
		
		String hourInstance = misc.getHour(timeInstance);
		String minInstance = misc.getMinute(timeInstance);
		String dateInstance = misc.getDate(timeInstance);
		String monthInstance = misc.getMonth(timeInstance);
		String yearInstance = misc.getYear(timeInstance);
		
		if(MainActivity.conn) {
			String cmd = "";
			String data = "";
			if(scheduleCheck.isChecked()) {
				cmd = newScheduleCmd;
				if(absCheck.isChecked()) {
					if(Integer.parseInt(yearInstance) > Integer.parseInt(yearIn.getText().toString())) {
						Toast.makeText(ScheduleActivity.this, "Invalid year!", Toast.LENGTH_SHORT).show();
						return;
					}else if(Integer.parseInt(monthInstance) > Integer.parseInt(monthIn.getText().toString())) {
						Toast.makeText(ScheduleActivity.this, "Invalid month!", Toast.LENGTH_SHORT).show();
						return;
					}else if(Integer.parseInt(dateInstance) > Integer.parseInt(dateIn.getText().toString())) {
						Toast.makeText(ScheduleActivity.this, "Invalid date!", Toast.LENGTH_SHORT).show();
						return;
					}else if(Integer.parseInt(hourInstance) > Integer.parseInt(hourIn.getText().toString())) {
						Toast.makeText(ScheduleActivity.this, "Invalid hour!", Toast.LENGTH_SHORT).show();
						return;
					}else if(Integer.parseInt(minInstance) > Integer.parseInt(minIn.getText().toString())) {
						Toast.makeText(ScheduleActivity.this, "Invalid minute!", Toast.LENGTH_SHORT).show();
						return;
					}
					data = "1schedule1abs" + getTimeFromText() + getDateFromText();
				}else {
					data = "1schedule0abs" + getTimeFromText();
				}
			}else {
				cmd = clearScheduleCmd;
			}
			
			/* Create and send frame */
			sendApply(createDataFrame(cmd, data));
		}else
			Toast.makeText(ScheduleActivity.this, "No connection. Please connect again!", Toast.LENGTH_SHORT).show();
		
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
	
	public String getTimeFromText() {
		String time = hourIn.getText().toString() + minIn.getText().toString();
		
		return time;
	}
	
	public String getDateFromText() {
		String date = dateIn.getText().toString() + monthIn.getText().toString() + yearIn.getText().toString();
		
		return date;
	}
	
	public void sendApply(String data) {
		/* Connect to server */
		try {
			MainActivity.socketTemp = new Socket();
			MainActivity.socketTemp.bind(null);
			if(MainActivity.useIpDefault)
				MainActivity.socketTemp.connect((new InetSocketAddress(SERVER_IP_DEFAULT, SERVER_PORT)), 10000);
			else
				MainActivity.socketTemp.connect((new InetSocketAddress(MainActivity.SERVER_IP, SERVER_PORT)), 10000);
        }catch(Exception e) {
            e.printStackTrace();
            return;
        }
		/* Open a output stream and a input stream to send and receive data */
        try{
        	PrintWriter out = new PrintWriter(MainActivity.socketTemp.getOutputStream(), true);
            /* Send frame to server */
            out.println(data);
        }catch(Exception e) {
            e.printStackTrace();
        }
	}

}
