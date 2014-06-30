package dhbk.android.topomanager;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.regex.Pattern;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.Html;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	public Socket clientSocket;
	public static String DATA;
	
	private final int SERVER_PORT = 9999;
	private final String SERVER_IP_DEFAULT = "192.168.150.1";
	private static String SERVER_IP = "";
	private final String scanCmd = "1";
	private final String measureCmd = "4";
	private final String detailBpCmd = "5";
	private final String detailHrCmd = "7";
	private final String detailHeightCmd = "9";
	private final String detailWeightCmd = "B";
	private final String detailAncetedentCmd = "D";
	private final String predictCmd = "F";
	private final String newScheduleCmd = "11";
	private final String clearScheduleCmd = "12";
	private static boolean useIpDefault = true;
	
	private static Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	Log.d("arg1", (String)msg.obj);
    	}
    }; 
	
	private Thread background = new Thread(new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			try {
//            	while(true){
                	Message msg = handler.obtainMessage(1, "string");
                	handler.sendMessage(msg);
//            	}
            }catch(Throwable t) {
            	Log.d("err", "err");
            }
		}
	});
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

//		Button measureBtn = (Button)findViewById(R.id.btnMeasure);
//		Button detailBtn = (Button)findViewById(R.id.btnDetail);
		/* If have no node, disable the Measure and Detail button */
//		measureBtn.setClickable(false);
//		detailBtn.setClickable(false);
		
		final ListView topoView = (ListView)findViewById(R.id.viewTopo);
		
		ArrayList<String> list = new ArrayList<String>();
		list.add("Node ID: 1\tPatient's ID: 1\n\tPham Huu Dang Nhat");
		list.add("Node ID: 2\tPatient's ID: 2\n\tNguyen Van Hien");

		final ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
					android.R.layout.simple_list_item_1, list);
		topoView.setAdapter(adapter);
		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent,
					View view, int position, long id) {
				// TODO Auto-generated method stub
				
				for(int i = 0; i < parent.getChildCount(); i++) {
					parent.getChildAt(i).setBackgroundColor(Color.WHITE);
				}
				view.setBackgroundColor(Color.CYAN);
			}
		});
		
		background.start();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		
		return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
	    // Handle item selection
	    switch (item.getItemId()) {
        case R.id.action_setIp:
            showSetServerIp();
            return true;
        case R.id.action_about:
            showAboutUs();
            return true;
        default:
            return super.onOptionsItemSelected(item);
	    }
	}
	
	/* Handle action_setIp option */
	public void showSetServerIp() {
		final Pattern IP_ADDRESS = Pattern.compile(
	        "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])\\.(25[0-5]|2[0-4]"
	        + "[0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]"
	        + "[0-9]{2}|[1-9][0-9]|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}"
	        + "|[1-9][0-9]|[0-9]))");
		
		/* Prompt user to get the server IP */
		AlertDialog.Builder alert = new AlertDialog.Builder(MainActivity.this);
		alert.setTitle("Server IP");
		alert.setMessage("Enter the IP address of server");
		final EditText input = new EditText(MainActivity.this);
		input.setHint("Default: " + SERVER_IP_DEFAULT);
		alert.setView(input);
		
		alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				Editable value = input.getText();
				SERVER_IP = String.valueOf(value);
				if(IP_ADDRESS.matcher(SERVER_IP).matches()) {
					useIpDefault = false;
					Toast.makeText(MainActivity.this, "Use the new IP address!", Toast.LENGTH_SHORT).show();
				}else {
					useIpDefault = true;
					Toast.makeText(MainActivity.this, "Invalid IP address! Use default IP.", Toast.LENGTH_SHORT).show();
				}
			}
		});
		
		alert.setNeutralButton("Default", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				Toast.makeText(MainActivity.this, "Use default IP.", Toast.LENGTH_SHORT).show();
				useIpDefault = true;
			}
		});

		alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				
			}
		});
		
		alert.show();
		
		return;
	}
	
	/* Handle action_about option */
	public void showAboutUs() {
		AlertDialog.Builder aboutUs = new AlertDialog.Builder(MainActivity.this);
		final TextView usView = new TextView(MainActivity.this);
		ScrollView scrView = new ScrollView(MainActivity.this);
		
		String htmlParagraph = "<h3>Ho Chi Minh City University of Technology</h3>"
							+ "<p><strong>Nguyen Van Hien</strong></p>"
							+ "<p><a href=\"\">nvhien1992@gmail.com</></p>"
							+ "<p><strong>Pham Huu Dang Nhat</strong></p>"
							+ "<p><a href=\"\">phamhuudangnhat@gmail.com</></p>"
							+ "<p><strong>Nguyen Viet Tien</strong></p>"
							+ "<p><a href=\"\">viettien56@gmail.com</></p>";
		
		usView.setText(Html.fromHtml(htmlParagraph));
		usView.setTextColor(new ColorStateList(new int[][]{new int[]{}}, new int [] {Color.WHITE}));
		scrView.addView(usView);
		
		aboutUs.setTitle("About us");
		aboutUs.setView(scrView);
		aboutUs.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				Toast.makeText(MainActivity.this, "Thanks!", Toast.LENGTH_SHORT).show();
			}
		});
		aboutUs.show();
		return;
	}
	
	/* Handle Measure button click */
	public void measure(View viewClick) {
		if(useIpDefault) {
			new socketWorker("").execute(SERVER_IP_DEFAULT, measureCmd);
		}else {
			new socketWorker("").execute(SERVER_IP, measureCmd);
		}
		
		return;
	}
	
	/* Handle Detail button click */
	public void detailNode(View viewClick) {
//		if(useIpDefault) {
//			new socketWorker("").execute(SERVER_IP_DEFAULT, detailBpCmd);
//		}else {
//			new socketWorker("").execute(SERVER_IP, detailBpCmd);
//		}
		Intent activityIntent = new Intent(this, DetailActivity.class);
		startActivity(activityIntent);
		
		return;
	}
	
	/* Handle Predict button click */
	public void predict(View viewClick) {
		if(useIpDefault) {
			new socketWorker("").execute(SERVER_IP_DEFAULT, predictCmd);
		}else {
			new socketWorker("").execute(SERVER_IP, predictCmd);
		}
		
		return;		
	}
	
	/* Handle Schedule button click */
	public void schedule(View viewClick) {
		
		if(useIpDefault) {
			new socketWorker("").execute(SERVER_IP_DEFAULT, newScheduleCmd);
		}else {
			new socketWorker("").execute(SERVER_IP, newScheduleCmd);
		}
		
		return;		
	}
		
	/* Process transceiver */
	public class socketWorker extends AsyncTask<String, Void, String> {
		private int errno = 0;
		private ListView topoView = (ListView)findViewById(R.id.viewTopo);
		private Button scheduleBtn = (Button)findViewById(R.id.btnSchedule);
		private Button measureBtn = (Button)findViewById(R.id.btnMeasure);
		private Button detailBtn = (Button)findViewById(R.id.btnDetail);
    	private ProgressDialog progDialog;
    	
    	String result = "";
    	String data = "";	//data in a frame.
    	
    	public socketWorker(String data){
    		this.data = data;
    	}
    	
		@Override
		protected void onPreExecute() {			
			progDialog = ProgressDialog.show(MainActivity.this, "Loading", "Please wait...", true);
		}
		
		@Override
		protected String doInBackground(String... params) {
			/* Try to connecting to the server */
			try {
				clientSocket = new Socket();
				clientSocket.bind(null);
				clientSocket.connect((new InetSocketAddress(params[0], SERVER_PORT)), 1000);
	        }catch(Exception e) {
	        	errno = 1;
	            e.printStackTrace();
	            return null;
	        }
			
			/* Disable al buttons while processing transceiver data */
	    	scheduleBtn.setClickable(false);
	    	measureBtn.setClickable(false);
	    	detailBtn.setClickable(false);
	    	
			/* If code segment below be reached, starting transceiver data */
	        try{
	        	PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
	        	BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
	            
	            /* Create a frame */
	            DATA = createDataFrame(params[1], data);
//	            Log.d("data", DATA);
	            
	            result += params[1];
	            
	            /* Send frame to server */
	            out.println(DATA);
	            
	            /* Read topology data from server */
	            String line = "";
	            while(true) {
	            	line = in.readLine().toString();
	            	if(line.equalsIgnoreCase("end")) break;
	            	result += line;
	            }
	            /* Close all streams */
	            out.close();
	            in.close();
	            clientSocket.close();
	            
	            /* Enable all buttons */
		        scheduleBtn.setClickable(true);
		        measureBtn.setClickable(true);
		        detailBtn.setClickable(true);
		        
	            return result;
	        }catch(Exception e) {
	        	errno = 2;
	        	
	        	/* Enable all buttons */
	        	scheduleBtn.setClickable(true);
		        measureBtn.setClickable(true);
		        detailBtn.setClickable(true);
		        
	            e.printStackTrace();
	        }
	        
			return null;
		}
		
		@Override
	    protected void onPostExecute(String result) {
			progDialog.dismiss();
			if(errno == 1) { //Connection error occur.
				Toast.makeText(MainActivity.this, "Can not connect!", Toast.LENGTH_SHORT).show();
			}else if(errno == 2) { //Reading error occur.
				Toast.makeText(MainActivity.this, "Reading error!", Toast.LENGTH_SHORT).show();
			}else {
				Toast.makeText(MainActivity.this, "Done!", Toast.LENGTH_SHORT).show();
				
				//TODO
//				String cmd = result.substring(0, 1); //get cmd.
//				if(cmd.equals(scanAct)) {
//					ArrayList<String> list = new ArrayList<String>();
//					
//					final ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
//								android.R.layout.simple_list_item_1, list);
//					topoView.setAdapter(adapter);
//					topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
//
//						@Override
//						public void onItemClick(AdapterView<?> parent,
//								View view, int position, long id) {
//							// TODO Auto-generated method stub
//							view.setBackgroundColor(Color.CYAN);
//							for(int i = 0; i < adapter.getCount(); i++) {
//								if(i != position)
//									topoView.getChildAt(i).setBackgroundColor(Color.WHITE);
//							}
//						}
//					});
//				}else if(cmd.equals(measureAct)) {
//					
//				}else if(cmd.equals(detailAct)) {
//					
//				}else if(cmd.equals(databaseAct)) {
//					
//				}
			}
	    }
		
	}
	
	public String createDataFrame(String cmd, String data) {
		String frame = "";
		int lengthData = data.length();

		frame += String.valueOf(lengthData) + cmd + data;
		
		return frame;
	}

}
