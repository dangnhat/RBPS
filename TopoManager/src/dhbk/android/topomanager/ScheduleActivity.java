package dhbk.android.topomanager;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;

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
	int nodeID;
	final String SERVER_IP_DEFAULT = "192.168.150.1";
	private final char new_schedule = 0x0013;
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
		char[] scheduleInfo = bundle.getCharArray("schedule");
		nodeID = bundle.getInt("nID");
		
		char isAbsChar = scheduleInfo[7];
		char isRelativeChar = scheduleInfo[14];
		char hourAbs, minAbs, date, month, hourRel, minRel;
		char[] yearArr;
		hourAbs = scheduleInfo[8];
		minAbs = scheduleInfo[9];
		date = scheduleInfo[10];
		month = scheduleInfo[11];
		yearArr = misc.subCharArray(scheduleInfo, 12, 2);
		int year = misc.digitCharArray2Int(yearArr);
		hourRel = scheduleInfo[15];
		minRel = scheduleInfo[16];
		
		if(isAbsChar == 1 || isRelativeChar == 1) {
			bigLayout.setVisibility(View.VISIBLE);
			scheduleCheck.setChecked(true);
			if(isAbsChar == 1) {
				absCheck.setChecked(true);
				relativeCheck.setChecked(false);
				dateLayout.setVisibility(View.VISIBLE);
				hourIn.setText(String.valueOf((int)hourAbs));
				minIn.setText(String.valueOf((int)minAbs));
				dateIn.setText(String.valueOf((int)date));
				monthIn.setText(String.valueOf((int)month));
				yearIn.setText(String.valueOf(year));
			}else if(isRelativeChar == 1) {
				absCheck.setChecked(false);
				relativeCheck.setChecked(true);
				dateLayout.setVisibility(View.INVISIBLE);
				hourIn.setText(String.valueOf((int)hourRel));
				minIn.setText(String.valueOf((int)minRel));
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
//		SimpleDateFormat curFormater = new SimpleDateFormat("HHmmddMMyyyy"); 
//		Calendar cal = Calendar.getInstance();
//		String timeInstance = curFormater.format(cal.getTime());
//		
//		String hourInstance = misc.getHourFromString(timeInstance);
//		String minInstance = misc.getMinuteFromString(timeInstance);
//		String dateInstance = misc.getDateFromString(timeInstance);
//		String monthInstance = misc.getMonthFromString(timeInstance);
//		String yearInstance = misc.getYearFromString(timeInstance);
		
		if(MainActivity.conn) {
			char[] data = new char[14];
			for(int i = 0; i < data.length; i++) {
				data[i] = 0;
			}
			if(scheduleCheck.isChecked()) {
				int hour = Integer.parseInt(hourIn.getText().toString());
				int min = Integer.parseInt(minIn.getText().toString());
				char[] nIdArr = misc.int2charArray(nodeID);
				misc.insData2Arr(data, nIdArr, 0);
				if(absCheck.isChecked()) {
					int date = Integer.parseInt(dateIn.getText().toString());
					int month = Integer.parseInt(monthIn.getText().toString());
					int year = Integer.parseInt(yearIn.getText().toString());
					char[] yyyy = misc.int2charArray(year);
					
					data[4] = 1;
					data[5] = (char)hour;
					data[6] = (char)min;
					data[7] = (char)date;
					data[8] = (char)month;
					data[9] = yyyy[0];
					data[10] = yyyy[1];
					data[11] = 0;
					data[12] = data[13] = 0;
				}else {
					data[4] = 0;
					data[11] = 1;
					data[12] = (char)hour;
					data[13] = (char)min;
				}
			}else {
				data[4] = 0;
				data[11] = 0;
			}
//			Log.d("apply", misc.digitCharArray2String(data));
			/* Create and send frame */
			sendApply(misc.createDataFrame(new_schedule, data));
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
	
	public void sendApply(char[] data) {
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
        	PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(MainActivity.socketTemp.getOutputStream(), "ISO-8859-1")), true);
            /* Send frame to server */
            out.println(data);
        }catch(Exception e) {
            e.printStackTrace();
        }
	}

}
