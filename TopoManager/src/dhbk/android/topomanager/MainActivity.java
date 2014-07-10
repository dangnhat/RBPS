package dhbk.android.topomanager;

import java.io.BufferedReader;
import java.io.IOException;
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
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	public static Socket socketBackground;
	public static Socket socketTemp;
	public static PrintWriter out;
	public static BufferedReader in;
	public static String DATA = ""; //frame.
//	public static String RESULT = ""; // "CMDcDATA" leter c is in between command and data.
	public static String chosenPatientId = "";
	public static boolean conn = false;
	public static boolean useIpDefault = true;
	private Misc parser = new Misc();
	Intent activityIntent;
	
	/* Parameters of socket */
	public final int SERVER_PORT = 9999;
	public final String SERVER_IP_DEFAULT = "192.168.150.1";
	public static String SERVER_IP = "";
	
	/* List of commands */
	private final String scanCmd = "1";
	private final String measureCmd = "2";
	private final String detailCmd = "3";
	private final String predictCmd = "4";
	private final String scheduleCmd = "5";
	
	/* what of massages of handler */
	private final int report = 1;
	private final int streamError = 2;
	private final int wakeUp = 3;
	
	/* Variables for list view */
	private ArrayList<NodeInfo> arrayNode;
	private CustomListAdapter arrNodeAdapter;
	private ListView topoView;
	private String nullSysBp = "===";
	private String nullDiasBpHr = "==";
	
	/* Create monitor to manage synchronization of two threads */
	class MonitorObject {}
	MonitorObject monitorObject = new MonitorObject();
	
	/* Handle msg from threads and update view */
	private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	if(msg.what == wakeUp) {
        		notifyToast("Waked up", Toast.LENGTH_SHORT);
        	}else if(msg.what == streamError) {
        		notifyToast("Streams error. Please connect again!", Toast.LENGTH_SHORT);
        	}else {
        		String result = (String) msg.obj;
        		String data = parser.parse(result, "cmd", "");
        		Log.d("data", data);

        		String nID = parser.parse(data, "", "n");
        		String pID = parser.parse(data, "n", "p");
	        	String name = parser.parse(data, "p", "");

	        	/* Update node on list view */
	    		NodeInfo nInfo = new NodeInfo(nID, pID, name);
	    		nInfo.setBpValue(nullSysBp, nullDiasBpHr);
	    		nInfo.setHrValue(nullDiasBpHr);
	    		nInfo.setDateAndTimeStampOrNotice("", false);
	    		addNode(0, nInfo);
	    		
	    		nInfo = new NodeInfo("2", "2", "Nguyen Van Hien");
	    		nInfo.setBpValue("113", "69");
	    		nInfo.setHrValue("79");
	    		nInfo.setDateAndTimeStampOrNotice("Waiting...", false);
	    		addNode(1, nInfo);
	    		
	    		nInfo = new NodeInfo("3", "3", "Nguyen Van Hien");
	    		nInfo.setBpValue("113", "69");
	    		nInfo.setHrValue("79");
	    		nInfo.setDateAndTimeStampOrNotice("Waiting...", false);
	    		addNode(2, nInfo);
	    		
	    		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
	    			@Override
	    			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
	    				for(int i = 0; i < parent.getChildCount(); i++) {
	    					parent.getChildAt(i).setBackgroundResource(0);
	    				}
	    				chosenPatientId = arrayNode.get(position).getPatientId();
	    				notifyToast(chosenPatientId, Toast.LENGTH_SHORT);
	    				view.setBackgroundResource(R.drawable.gradient_bg_hover);
	    			}
	    		});
	    		/* </End> update node on list view */
        	}
    	}
    }; 
	
    /* Always connect to server */
	private Thread background = new Thread(new Runnable() {
		@Override
		public void run() {
			/* Transfer data */
			while(true) {
				if(!conn) {
					threadWait();
					Message msg = handler.obtainMessage(wakeUp);
		            handler.sendMessage(msg);
				}else {
					try{
			            String line = "";
			            String result = "";
			            while(conn) {	//do while still connect and no button is chosen.
			            	line = in.readLine().toString();
			            	if(line.equalsIgnoreCase("end")) {
			            		Log.d("result1", result);
					            Message msg = handler.obtainMessage(report, (Object)result);
					            handler.sendMessage(msg);
					            result = "";
			            	}
			            	result += line;
			            }
			        }catch(Exception e) {
			        	Message msg = handler.obtainMessage(streamError);
			            handler.sendMessage(msg);
			        }
				}
			}
		}
	});
	
	public void addNode(int index, NodeInfo node) {
		arrayNode.add(index, node);
		arrNodeAdapter.notifyDataSetChanged();
	}
	
	public void removeNode(int index) {
		if(arrNodeAdapter != null) {
			arrayNode.remove(index);
			arrNodeAdapter.notifyDataSetChanged();
		}
	}
	
	public void removeAllNode() {
		if(arrNodeAdapter != null) {
			int num = arrayNode.size();
			for(int i = num - 1; i >= 0; i--) {
				arrayNode.remove(i);
			}
			arrNodeAdapter.notifyDataSetChanged();
		}
	}
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		/* Initialize Adapter for topoView */
		topoView = (ListView)findViewById(R.id.viewTopo);
		arrayNode = new ArrayList<NodeInfo>();
		arrNodeAdapter = new CustomListAdapter(MainActivity.this, R.layout.list_view, arrayNode);
		topoView.setAdapter(arrNodeAdapter);

		/* Try to connecting to the server */
		conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
		if(conn)
			scanNode();

		/* Always run the background thread */
		background.start();
//		Message msg = handler.obtainMessage(report, (Object)"1cmd1n1pnvhien");
//        handler.sendMessage(msg);
	}
	
	@Override
	protected void onDestroy() {
        super.onDestroy();
        try {
        	conn = false;
        	socketBackground.close();
        	socketTemp.close();
		} catch (IOException e) {}
        notifyToast("Closed!", Toast.LENGTH_SHORT);
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
        case R.id.action_reconnect:
        	reconnect();
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
		if(useIpDefault)
			input.setHint("Default: " + SERVER_IP_DEFAULT);
		else 
			input.setText(SERVER_IP);
		alert.setView(input);
		
		alert.setPositiveButton("Reconnect", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				Editable value = input.getText();
				SERVER_IP = String.valueOf(value);
							
				/* Check validation of the new IP */
				if(IP_ADDRESS.matcher(SERVER_IP).matches()) {
					notifyToast("Use the new IP address!", Toast.LENGTH_SHORT);
					useIpDefault = false;
				}else {
					notifyToast("Invalid IP address! Use default IP.", Toast.LENGTH_SHORT);
					useIpDefault = true;
				}
				
				reconnect();
			}
		});
		
		alert.setNeutralButton("Default", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				notifyToast("Use default IP. Reconnecting.", Toast.LENGTH_SHORT);
				useIpDefault = true;
				input.setText("");
				reconnect();
			}
		});

		alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				
			}
		});
		
		alert.show();
		
		return;
	}
	
	/* Handle action_reconnect option */
	public void reconnect() {
		conn = false;
		removeAllNode();
		/* close socket before */
		try {
			socketBackground.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		
		/* Reconnect */
		conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
		if(conn) {
			threadRun();
			scanNode();
		}

		return;
	}
	
	/* Handle action_about option */
	public void showAboutUs() {
		AlertDialog.Builder aboutUs = new AlertDialog.Builder(MainActivity.this);
		final TextView usView = new TextView(MainActivity.this);
		ScrollView scrView = new ScrollView(MainActivity.this);
		
		String htmlParagraph = "<h3><font color=\"red\">Ho Chi Minh City University of Technology</font></h3>"
							+ "<p><strong><font color=\"green\">Nguyen Van Hien</font></strong><br>"
							+ "<a href=\"\">nvhien1992@gmail.com</></p>"
							
							+ "<p><strong><font color=\"green\">Pham Huu Dang Nhat</font></strong><br>"
							+ "<a href=\"\">phamhuudangnhat@gmail.com</></p>"
							
							+ "<p><strong><font color=\"green\">Nguyen Viet Tien</font></strong><br>"
							+ "<a href=\"\">viettien56@gmail.com</></p>";
		
		usView.setBackgroundColor(Color.BLACK);
		usView.setText(Html.fromHtml(htmlParagraph));
		usView.setTextColor(new ColorStateList(new int[][]{new int[]{}}, new int [] {Color.WHITE}));
		scrView.addView(usView);
		
		aboutUs.setTitle("About us");
		aboutUs.setView(scrView);
		aboutUs.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				notifyToast("Thanks!", Toast.LENGTH_SHORT);
			}
		});
		aboutUs.show();
		return;
	}
	
	public void scanNode() {
		DATA = createDataFrame(scanCmd, "");
		try{
			out.println(DATA);
		}catch(Exception e){
			
		}
	}
	
	/* Handle Measure button click */
	public void measure(View viewClick) {
		if(conn) {
			if(chosenPatientId.equals("")) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else {
				/* Create a frame */
	            DATA = createDataFrame(measureCmd, chosenPatientId);
				new socketWorker(DATA).execute();
			}
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		
		return;
	}
	
	/* Handle Detail button click */
	public void detailNode(View viewClick) {
		if(conn) {
			if(chosenPatientId.equals("")) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else {
				/* Create a frame */
	            DATA = createDataFrame(detailCmd, chosenPatientId);
				new socketWorker(DATA).execute();
				
				activityIntent = new Intent(this, DetailActivity.class);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
//			RESULT = "1n1pNvhien"
//					+ "recently110sys66dias78hr164010072014"
//					+ "avgdate112sys69dias69hrpeakdate113sys70dias70hr124007072014130306072014173408072014"
//					+ "avgweek112sys69dias69hrpeakweek113sys70dias70hr124007072014130306072014173408072014"
//					+ "avgmonth112sys69dias69hrpeakmonth113sys70dias70hr124007072014130306072014173408072014"
//					+ "height170cm144504062013"
//					+ "weight60kg130407072014"
//					+ "history0000";
//			Intent activityIntent = new Intent(this, DetailActivity.class);
//			activityIntent.putExtras(createBundle("detailInfo", RESULT));
//			startActivity(activityIntent);
		}
		
		return;
	}
	
	/* Handle Predict button click */
	public void predict(View viewClick) {
		if(conn) {
			if(chosenPatientId.equals("")) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else {
				/* Create a frame */
	            DATA = createDataFrame(predictCmd, chosenPatientId);
	            new socketWorker(DATA).execute();
	            
				activityIntent = new Intent(this, PredictActivity.class);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
//			RESULT = "1n1pNvhien"
//					+ "prehyper0"
//					+ "avgmonth137sys69dias69hr21.5"
//					+ "history0001";
//			Intent activityIntent = new Intent(this, PredictActivity.class);
//			activityIntent.putExtras(createBundle("predictInfo", RESULT));
//			startActivity(activityIntent);
		}
		
		return;		
	}
	
	/* Handle Schedule button click */
	public void schedule(View viewClick) {
		if(conn) {
			if(chosenPatientId.equals("")) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else {
				/* Create a frame */
	            DATA = createDataFrame(scheduleCmd, chosenPatientId);
				new socketWorker(DATA).execute();
				
				activityIntent = new Intent(this, ScheduleActivity.class);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
//			RESULT = "1schedule"
//					+ "0abs124509072014";
//			Intent activityIntent = new Intent(this, ScheduleActivity.class);
//			activityIntent.putExtras(createBundle("scheduleInfo", RESULT));
//			startActivity(activityIntent);
		}
		
		return;		
	}
		
	/* Process transceiver when choosing buttons */
	public class socketWorker extends AsyncTask<String, Void, String> {
		private int errno = 0;
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
			/* Connect to server */
			try {
				socketTemp = new Socket();
				socketTemp.bind(null);
				if(useIpDefault)
					socketTemp.connect((new InetSocketAddress(SERVER_IP_DEFAULT, SERVER_PORT)), 10000);
				else
					socketTemp.connect((new InetSocketAddress(SERVER_IP, SERVER_PORT)), 10000);
	        }catch(Exception e) {
	        	errno = 1;
	            e.printStackTrace();
	            return null;
	        }
			/* Open a output stream and a input stream to send and receive data */
	        try{
	        	PrintWriter out = new PrintWriter(socketTemp.getOutputStream(), true);
	        	BufferedReader in = new BufferedReader(new InputStreamReader(socketTemp.getInputStream()));
	            /* Send frame to server */
	            out.println(this.data);
	            
	            /* Read topology data from server */
	            String line = "";
	            while(true) {
	            	line = in.readLine().toString();
	            	if(line.equalsIgnoreCase("end")) break;
	            	result += line;
	            }
	            
	            return result;
	        }catch(Exception e) {
	        	errno = 2;
	            e.printStackTrace();
	        }
	        
			return null;
		}
		
		@Override
	    protected void onPostExecute(String result) {
			progDialog.dismiss();
			try {
				socketTemp.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			if(errno == 1) {
				notifyToast("Can not connect!", Toast.LENGTH_SHORT);
			}else if(errno == 2) { //Transfering error occur.
				notifyToast("Transfering error! Please connect again!", Toast.LENGTH_SHORT);
			}else {
				notifyToast("Done!", Toast.LENGTH_SHORT);
				activityIntent.putExtras(createBundle("data", result));
				startActivity(activityIntent);
			}
	    }
	}
	
	/* Create a frame */
	public String createDataFrame(String cmd, String data) {
		String frame = "";
		int lengthData = data.length();
		frame += String.valueOf(lengthData) + cmd + data;
		
		return frame;
	}
	
	/* Try to connecting to server */
	public boolean tryConnect(boolean useIpDefault, String defaultIP, String newIP) {
		try {
			socketBackground = new Socket();
			socketBackground.bind(null);
			if(useIpDefault)
				socketBackground.connect((new InetSocketAddress(defaultIP, SERVER_PORT)), 10000);
			else
				socketBackground.connect((new InetSocketAddress(newIP, SERVER_PORT)), 10000);
        }catch(Exception e) {
        	notifyToast("Can not connect!", Toast.LENGTH_SHORT);
            return false;
        }
		
		/* Open streams */
		try {
			out = new PrintWriter(socketBackground.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(socketBackground.getInputStream()));
		} catch (IOException e) {
			return false;
		}
		notifyToast("Connected!", Toast.LENGTH_SHORT);
		
		return true;
	}

	/* Toast */
	public void notifyToast(String noice, int timeShow) {
		Toast.makeText(getBaseContext(), noice, timeShow).show();
		
		return;
	}
	
	public Bundle createBundle(String key, String extra) {
		Bundle bundle = new Bundle();
		bundle.putString(key, extra);
		
		return bundle;
	}
	
	public void threadWait() {
		/* Waiting for notice wakeup*/
		synchronized (monitorObject) {
			try {				            
				monitorObject.wait();
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
		}
	}
	
	public void threadRun() {
		synchronized (monitorObject) {
			monitorObject.notify();
		}
	}
	
}
