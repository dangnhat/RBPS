package dhbk.android.topomanager;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
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
	public static char[] FRAME = new char[258]; //frame.
	public static int chosenPatientId = -1;
	public static int chosenNodeId = -1;
	public final int maxSizeBuffer = 258;
	public static boolean conn = false;
	public static boolean useIpDefault = true;
	private Misc parser = new Misc();
	Intent activityIntent;
	
	/* Parameters of socket */
	public final int SERVER_PORT = 9999;
	public final String SERVER_IP_DEFAULT = "192.168.150.1";
	public static String SERVER_IP = "";
	
	/* List of commands */
	private final int scan = 0x0001;
	private final int measure = 0x0004;
	private final int detail_info = 0x0014;
	private final int detail_bp = 0x0005;
	private final int detail_hr = 0x0007;
	private final int detail_height = 0x0009;
	private final int detail_weight = 0x000B;
	private final int detail_history = 0x000D;
	private final int predict = 0x000F;
	private final int node_report = 0x0017;
	private final int udt_node = 0x0003;
	private final int udt_schedule = 0x0013;
	private final int get_schedule = 0x0019;
	
	/* Length */
	private final int pidLeng = 4;
	private final int nidLeng = 4;
	private final int nameLeng = 20;
	
	/* what of massages of handler */
	private final int scanMsg = 1;
	private final int updateMsg = 2;
	private final int streamErrorMsg = 3;
	private final int wakeUpMsg = 4;
	private final int measureMsg = 5;
	
	/* Variables for list view */
	private ArrayList<NodeInfo> arrayNode;
	private CustomListAdapter arrNodeAdapter;
	private ListView topoView;
	
	/* Create monitor to manage synchronization of two threads */
	class MonitorObject {}
	MonitorObject monitorObject = new MonitorObject();
	
	/* Handle msg from threads and update view */
	private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	if(msg.what == wakeUpMsg) {
        		notifyToast("Waked up", Toast.LENGTH_SHORT);
        	}else if(msg.what == streamErrorMsg) {
        		notifyToast("Streams error. Please connect again!", Toast.LENGTH_SHORT);
        	}else if(msg.what == scanMsg){
        		char[] scanReport = (char[])msg.obj;
        		char[] nodeId = parser.subCharArray(scanReport, 0, 4);
        		char[] patientId = parser.subCharArray(scanReport, 4, 4);
        		char[] nameArr = parser.subCharArray(scanReport, 8, 20);

        		char isSchedule = scanReport[28];
        		boolean isSche = false;
        		if(isSchedule == 1) 
        			isSche = true;
        		
        		int nID = parser.digitCharArray2Int(nodeId);
        		int pID = parser.digitCharArray2Int(patientId);
        		String name = parser.concatNameArr2String(nameArr, nameLeng);
        		
        		NodeInfo nInfo = new NodeInfo(nID, pID, name);
        		nInfo.setIsSchedule(isSche);
        		addNode(nInfo);
        	}else if(msg.what == updateMsg) {
        		char[] udtMsg = (char[])msg.obj;
        		char[] cmdArr = parser.subCharArray(udtMsg, 1, 2);
        		int cmd = parser.digitCharArray2Int(cmdArr);
        		char[] nodeId = parser.subCharArray(udtMsg, 3, 4);
        		int nID = parser.digitCharArray2Int(nodeId);
        		Log.d("nIdUpade", String.valueOf(nID));
        		NodeInfo nInfo = searchNodeFromNid(nID);
        		
        		if(cmd == node_report) {	//process the report from nodes.
        			Log.d("rptnode", "rpt");
	        		char[] sysBpArr = parser.subCharArray(udtMsg, 7, 4);
	        		char[] diasBpArr = parser.subCharArray(udtMsg, 11, 4);
	        		char[] hrArr = parser.subCharArray(udtMsg, 15, 4);
	        		char[] timeArr = parser.subCharArray(udtMsg, 19, 6);
	        		
	        		char[] yearArr = parser.subCharArray(timeArr, 4, 2);
	        		
	        		int sysBp = parser.digitCharArray2Int(sysBpArr);
	        		int diasBp = parser.digitCharArray2Int(diasBpArr);
	        		int hr = parser.digitCharArray2Int(hrArr);
	        		int hour = (int)timeArr[0];
	        		int min = (int)timeArr[1];
	        		int date = (int)timeArr[2];
	        		int month = (int)timeArr[3];
	        		int year = parser.digitCharArray2Int(yearArr);
	        		
	        		if(nInfo != null) {
		        		nInfo.setBpHrValue(sysBp, diasBp, hr);
		        		nInfo.setDateAndTimeStamp(hour, min, date, month, year);
		        		nInfo.setNotice("");
		        		arrNodeAdapter.notifyDataSetChanged();
	        		}
        		}else if(cmd == udt_node) {	//Update num of node and info of each node.
        			Log.d("udtnode", "udt");
        			char[] patientId = parser.subCharArray(udtMsg, 7, 4);
            		int pID = parser.digitCharArray2Int(patientId);
            		if(chosenNodeId == nID)
            			chosenPatientId = pID;
            		String name = "";
            		if(pID != 0) {
	            		FRAME = parser.createDataFrame(detail_info, patientId);
	    				char[] result = sendReceiveFrame(FRAME);
	    				
	    				char[] nameArr = parser.subCharArray(result, 7, 20);
	    				name = parser.concatNameArr2String(nameArr, nameLeng);
            		}
        			if(nInfo != null) {
        				Log.d("new node", "ok");
        				nInfo.setPatientId(pID);
        				nInfo.setName(name);
        			}else {
        				Log.d("udt old node", "ok");
        				nInfo = new NodeInfo(nID, pID, name);
        				nInfo.setIsSchedule(false);
        				addNode(nInfo);
        			}
        			arrNodeAdapter.notifyDataSetChanged();
        		}else if(cmd == udt_schedule) { //Update schedule for all node when a node is scheduled.
        			Log.d("udtschenode", "sche");
        			char isAbsChar = udtMsg[7];
        			char isRelativeChar = udtMsg[14];
        			Log.d("isSchedule", String.valueOf((int)isAbsChar)+", "+String.valueOf((int)isRelativeChar));

        			if(nInfo != null) {
        				Log.d("nodeIdNotNull", "not null");
        				if(isAbsChar == 1 || isRelativeChar == 1) {
        					nInfo.setIsSchedule(true);
        					arrNodeAdapter.notifyDataSetChanged();
        				}
        			}
        		}
        	}else if(msg.what == measureMsg) {
        		int nID = msg.arg1;
        		Log.d("measuring node", String.valueOf(nID));
        		NodeInfo nInfo = searchNodeFromNid(nID);
        		if(nInfo != null) {
        			nInfo.setNotice("Measuring...");
        			arrNodeAdapter.notifyDataSetChanged();
        		}
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
					Message msg = handler.obtainMessage(wakeUpMsg);
		            handler.sendMessage(msg);
				}else {
					scanNode();
					char[] backgroudCmd = new char[maxSizeBuffer+1];
					backgroudCmd[maxSizeBuffer] = 1;
					out.println(backgroudCmd);
					try{
			            char[] ret;
			            while(conn) {	//do while still connect.
			            	ret = new char[maxSizeBuffer];
			            	in.read(ret);
		            		Message msg = handler.obtainMessage(updateMsg, (Object)ret);
				            handler.sendMessage(msg);
			            }
			        }catch(Exception e) {
			        	Message msg = handler.obtainMessage(streamErrorMsg);
			            handler.sendMessage(msg);
			            e.printStackTrace();
			        }
				}
			}
		}
	});
	
	public void addNode(NodeInfo node) {
		arrayNode.add(node);
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
		
		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				for(int i = 0; i < parent.getChildCount(); i++) {
					parent.getChildAt(i).setBackgroundResource(0);
				}
				chosenPatientId = arrayNode.get(position).getPatientId();
				chosenNodeId = arrayNode.get(position).getNodeId();
				notifyToast(String.valueOf(chosenPatientId), Toast.LENGTH_SHORT);
				view.setBackgroundResource(R.drawable.gradient_bg_hover);
			}
		});

		/* Try to connecting to the server */
		conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
		
		/* Always run the background thread */
		background.start();
	}
	
	@Override
	protected void onDestroy() {
        super.onDestroy();
        char[] backgroudEndCmd = new char[maxSizeBuffer+1];
        backgroudEndCmd[maxSizeBuffer] = 2; //terminate background thread.
        if(conn) {
			out.println(backgroudEndCmd);
        }
        try {
        	conn = false;
        	socketBackground.close();
        	if(socketTemp != null) {
        		socketTemp.close();
        		notifyToast("Closed socket temp!", Toast.LENGTH_SHORT);
        	}
		} catch (IOException e) {
			e.printStackTrace();
		}
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
							+ "<p><strong><font color=\"white\">Nguyen Van Hien</font></strong><br>"
							+ "<a href=\"\">nvhien1992@gmail.com</></p>"
							
							+ "<p><strong><font color=\"white\">Pham Huu Dang Nhat</font></strong><br>"
							+ "<a href=\"\">phamhuudangnhat@gmail.com</></p>"
							
							+ "<p><strong><font color=\"white\">Nguyen Viet Tien</font></strong><br>"
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
		char[] data = new char[4];
		char[] scanNode = new char[maxSizeBuffer+1];
		
		/* Create frame without data */
		FRAME = parser.createDataFrame(scan, data);
		
		/* Send frame and get reply data */
		scanNode = sendReceiveFrame(FRAME);

		if(scanNode != null) {
			Log.d("scanData", "not null");
			int leng = (int)scanNode[0];
			/* Get number of node */
			int numOfNode = (leng-1)/8;
			Log.d("numnode", String.valueOf(numOfNode));
			/* Get detail information of all node */
			for(int i = 0; i < numOfNode; i++) {
				char[] scanReport = new char[4+4+20+1];
				char[] pID = new char[4];
				char[] nID = new char[4];
				
				/* Get nID and pId and put them to array destination */
				nID = parser.subCharArray(scanNode, 4+i*(nidLeng+pidLeng), nidLeng);
				pID = parser.subCharArray(scanNode, 4+nidLeng+i*(nidLeng+pidLeng), pidLeng);
				int pIdInt = parser.digitCharArray2Int(pID);
				parser.insData2Arr(scanReport, nID, 0);
				parser.insData2Arr(scanReport, pID, nidLeng);
				
				if(pIdInt != 0) {
					/* Get patient's name of each node */
					FRAME = parser.createDataFrame(detail_info, pID);
					FRAME = sendReceiveFrame(FRAME);
					
					/* Get name and put it to an array destination */
					char[] name = parser.subCharArray(FRAME, 7, nameLeng);
					parser.insData2Arr(scanReport, name, nidLeng+pidLeng);
				}
				
				/* Get scheduling information of each node */
				FRAME = parser.createDataFrame(get_schedule, nID);
				FRAME = sendReceiveFrame(FRAME);
				char isAbsChar = FRAME[7];
				char isRelativeChar = FRAME[14];
				if(isAbsChar == 1 || isRelativeChar == 1)
					scanReport[28] = 1;
				Message msg = handler.obtainMessage(scanMsg, (Object)scanReport);
	            handler.sendMessage(msg);
			}
		}else {
			Log.d("scanData", "null");
		}
		
		return;
	}
	
	/* Handle Measure button click */
	public void measure(View viewClick) {
		if(conn) {
			if(chosenPatientId == -1) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else if(chosenPatientId == 0){
				notifyToast("No patient!", Toast.LENGTH_SHORT);
			}else {
				/* Create a frame */
				char[] data = new char[nidLeng];
				data = parser.int2charArray(chosenNodeId);
				Log.d("choseNID0", String.valueOf((int)data[0]));
				Log.d("choseNID1", String.valueOf((int)data[1]));
				Log.d("choseNID2", String.valueOf((int)data[2]));
				Log.d("choseNID3", String.valueOf((int)data[3]));
	            FRAME = parser.createDataFrame(measure, data);
				sendReceiveFrame(FRAME);
				Log.d("chosennode", String.valueOf(chosenNodeId));
				Message msg = handler.obtainMessage(measureMsg, chosenNodeId, chosenPatientId);
	            handler.sendMessage(msg);
			}
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		
		return;
	}
	
	/* Handle Detail button click */
	public void detailNode(View viewClick) {
		if(conn) {
			if(chosenPatientId == -1) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else if(chosenPatientId == 0){
				notifyToast("No patient!", Toast.LENGTH_SHORT);
			}else {
				/* new intent */
				activityIntent = new Intent(this, DetailActivity.class);
				activityIntent.putExtra("nID", String.valueOf(chosenNodeId));
				activityIntent.putExtra("pID", String.valueOf(chosenPatientId));
				char[] result = new char[maxSizeBuffer+1];
				char[] data = new char[pidLeng];
				data = parser.int2charArray(chosenPatientId);
				/* Create frame and transfer */
	            FRAME = parser.createDataFrame(detail_info, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_info", result);
	            
	            FRAME = parser.createDataFrame(detail_bp, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_bp", result);
	            Log.d("detail_bp", parser.digitCharArray2String(result));
	            
	            FRAME = parser.createDataFrame(detail_hr, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_hr", result);
	            
	            FRAME = parser.createDataFrame(detail_height, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_height", result);
	            
	            FRAME = parser.createDataFrame(detail_weight, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_weight", result);
	            
	            FRAME = parser.createDataFrame(detail_history, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("detail_history", result);
	            startActivity(activityIntent);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		}
		
		return;
	}
	
	/* Handle Predict button click */
	public void predict(View viewClick) {
		if(conn) {
			if(chosenPatientId == -1) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else if(chosenPatientId == 0){
				notifyToast("No patient!", Toast.LENGTH_SHORT);
			}else {
				/* new intent */
				activityIntent = new Intent(this, PredictActivity.class);
				String basic_info = "";
				NodeInfo nInfo = searchNodeFromPid(chosenPatientId);
				basic_info += String.valueOf(chosenNodeId)+"n"+String.valueOf(chosenPatientId)+"p"+nInfo.getName();
				activityIntent.putExtra("basic_info", basic_info);
				
				char[] result = new char[maxSizeBuffer+1];
				char[] data = new char[pidLeng];
				data = parser.int2charArray(chosenPatientId);
				/* Create a frame */
	            FRAME = parser.createDataFrame(predict, data);
	            result = sendReceiveFrame(FRAME);
	            activityIntent.putExtra("predict", result);
	            startActivity(activityIntent);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		}
		
		return;		
	}
	
	/* Handle Schedule button click */
	public void schedule(View viewClick) {
		if(conn) {
			if(chosenPatientId == -1) {
				notifyToast("Please choose a patient!", Toast.LENGTH_SHORT);
			}else if(chosenPatientId == 0){
				notifyToast("No patient!", Toast.LENGTH_SHORT);
			}else {
				ProgressDialog progDialog = ProgressDialog.show(MainActivity.this, "Loading", "Please wait...", true);
				/* new intent */
				activityIntent = new Intent(this, ScheduleActivity.class);
				activityIntent.putExtra("nID", chosenNodeId);
				char[] result = new char[maxSizeBuffer];
				char[] data = new char[nidLeng];
				data = parser.int2charArray(chosenNodeId);
				/* Create a frame */
	            FRAME = parser.createDataFrame(get_schedule, data);
	            result = sendReceiveFrame(FRAME);
				activityIntent.putExtra("schedule", result);
				progDialog.dismiss();
				startActivity(activityIntent);
			}
		}else {
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		}
		
		return;		
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
        	e.printStackTrace();
        	notifyToast("Can not connect!", Toast.LENGTH_SHORT);
            return false;
        }
		
		/* Open streams */
		try {
			out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socketBackground.getOutputStream(), "ISO-8859-1")), true);
			in = new BufferedReader(new InputStreamReader(socketBackground.getInputStream(), "ISO-8859-1"));
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
	
	public Bundle createBundle(String key, char[] extra) {
		Bundle bundle = new Bundle();
		bundle.putCharArray(key, extra);
		
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
	
	public NodeInfo searchNodeFromPid(int pID) {
		NodeInfo chosenNode = null;
		for(int i = 0; i < arrayNode.size(); i++) {
			if(arrayNode.get(i).getPatientId() == pID)
				chosenNode = arrayNode.get(i);
		} 
		
		return chosenNode;
	}
	
	public NodeInfo searchNodeFromNid(int nID) {
		NodeInfo chosenNode = null;
		Log.d("numEinList", String.valueOf(arrayNode.size()));
		for(int i = 0; i < arrayNode.size(); i++) {
			Log.d("nodeInArr", String.valueOf(arrayNode.get(i).getNodeId()));
			if(arrayNode.get(i).getNodeId() == nID)
				chosenNode = arrayNode.get(i);
		} 
		
		return chosenNode;
	}
	
	public char[] sendReceiveFrame(char[] frame) {
		char[] cmdArr = parser.subCharArray(frame, 1, 2);
		int cmd = parser.digitCharArray2Int(cmdArr);
		char[] result = new char[maxSizeBuffer];
		/* Connect to server */
		try {
			socketTemp = new Socket();
			socketTemp.bind(null);
			if(useIpDefault)
				socketTemp.connect((new InetSocketAddress(SERVER_IP_DEFAULT, SERVER_PORT)), 10000);
			else
				socketTemp.connect((new InetSocketAddress(SERVER_IP, SERVER_PORT)), 10000);
        }catch(Exception e) {
            e.printStackTrace();
            Log.d("conn", "can't conn");
            return null;
        }
		/* Open a output stream and a input stream to send and receive data */
        try{
        	PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socketTemp.getOutputStream(), "ISO-8859-1")), true);
        	BufferedReader in = new BufferedReader(new InputStreamReader(socketTemp.getInputStream(), "ISO-8859-1"));
            /* Send frame to server */
            out.println(frame);

            /* Read data from server */
            if(cmd != measure) {
            	in.read(result);
            }
        }catch(Exception e) {
        	Log.d("stream", "can't open");
            e.printStackTrace();
            try {
            	if(socketTemp != null)
            		socketTemp.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
            return null;
        }
        try {
        	if(socketTemp != null)
        		socketTemp.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
        return result;
	}
}
