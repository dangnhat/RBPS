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
import android.webkit.WebChromeClient.CustomViewCallback;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	public static Socket clientSocket;
	public static String DATA; //frame.
	
	/* Parameters of socket */
	private final int SERVER_PORT = 9999;
	private final String SERVER_IP_DEFAULT = "192.168.150.1";
	private static String SERVER_IP = "";
	
	/* List of commands */
	private final String scanCmd = "01";
	private final String measureCmd = "2";
	private final String detailCmd = "3";
	private final String predictCmd = "4";
	private final String scheduleCmd = "5";
	private final String newScheduleCmd = "11";
	private final String clearScheduleCmd = "12";
	
	private static boolean useIpDefault = true;
	private static boolean conn = false;
	private ArrayList<NodeInfo> arrayNode;
	private CustomListAdapter arrNodeAdapter;
	
	/* Create monitor to synchronize two threads */
	class MonitorObject {}
	MonitorObject monitorObject = new MonitorObject();
	
	/* Handle msg from threads and update view */
	private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
        	if(msg.what == 3) {
        		notifyToast("Waked up", Toast.LENGTH_SHORT);
        	}else if(msg.what == 2) {
        		notifyToast("Streams error. Please connect again!", Toast.LENGTH_SHORT);
        	}else {
	        	String name = (String)msg.obj;
	        	ListView topoView = (ListView)findViewById(R.id.viewTopo);
	        	
	    		ArrayList<String> list = new ArrayList<String>();
	    	
	    		list.add("Node ID: 1\tPatient's ID: 1\n\t"+name+"\nnvhien");
	    		list.add("Node ID: 2\tPatient's ID: 2\n\tNguyen Van Hien");
	    		
	    		ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this, android.R.layout.simple_list_item_1,list);
	    		topoView.setAdapter(adapter);
	    		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
	
	    			@Override
	    			public void onItemClick(AdapterView<?> parent,
	    					View view, int position, long id) {
	    				
	    				for(int i = 0; i < parent.getChildCount(); i++) {
	    					parent.getChildAt(i).setBackgroundColor(Color.WHITE);
	    				}
	    				view.setBackgroundColor(Color.CYAN);
	    			}
	    		});
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
					synchronized(monitorObject) {
						try {
							monitorObject.wait();
							Message msg = handler.obtainMessage(3);
				            handler.sendMessage(msg);
						} catch (InterruptedException e1) {
							e1.printStackTrace();
						}
					}
				}else {
					try{
			        	PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
			        	BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
			            
			        	out.println(scanCmd);
			        	
			            String line = "";
			            String result = "";
			            while(conn) {
			            	line = in.readLine().toString();
			            	if(line.equalsIgnoreCase("end")) break;
			            	result += line;
			            }
			            Message msg = handler.obtainMessage(1, (Object)result);
			            handler.sendMessage(msg);
			        }catch(Exception e) {
			        	Message msg = handler.obtainMessage(2);
			            handler.sendMessage(msg);
			            e.printStackTrace();
			        }
				}
			}
		}
	});
	
	public void addNode(NodeInfo node) {
		arrayNode.add(0, node);
		arrNodeAdapter.notifyDataSetChanged();
	}
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		/* Try to connecting to the server */
		conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);

		/* Always run the background thread */
//		background.start();
		
		ListView topoView = (ListView)findViewById(R.id.viewTopo);

		arrayNode = new ArrayList<NodeInfo>();
		arrNodeAdapter = new CustomListAdapter(this, R.layout.list_view, arrayNode);
		topoView.setAdapter(arrNodeAdapter);
		
		NodeInfo nInfo = new NodeInfo("1", "1", "Abc");
		nInfo.setBpValue("110/66");
		nInfo.setHrValue("77");
		nInfo.setTimestamp("12h30");
		this.addNode(nInfo);
		
		this.addNode(nInfo = new NodeInfo("2", "2", "XXX"));
		nInfo.setBpValue("113/68");
		nInfo.setHrValue("79");
		nInfo.setTimestamp("15h30");
//		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
//
//			@Override
//			public void onItemClick(AdapterView<?> parent,
//					View view, int position, long id) {
//				
//				for(int i = 0; i < parent.getChildCount(); i++) {
//					parent.getChildAt(i).setBackgroundColor(Color.WHITE);
//				}
//				view.setBackgroundColor(Color.CYAN);
//			}
//		});
	}
	
	@Override
	protected void onDestroy() {
        super.onDestroy();
        try {
        	conn = false;
			clientSocket.close();
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
				
				/* close socket before */
				try {
					clientSocket.close();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
				
				/* Reconnect */
				conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
				if(conn) {
					synchronized (monitorObject) {
						monitorObject.notify();
					}
				}
			}
		});
		
		alert.setNeutralButton("Default", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				notifyToast("Use default IP. Reconnecting.", Toast.LENGTH_SHORT);
				useIpDefault = true;
				input.setText("");
				
				/* close socket before */
				try {
					clientSocket.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				/* Reconnect */
				conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
				if(conn) {
					synchronized (monitorObject) {
						monitorObject.notify();
					}
				}
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
		/* close socket before */
		try {
			clientSocket.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		
		/* Reconnect */
		conn = tryConnect(useIpDefault, SERVER_IP_DEFAULT, SERVER_IP);
		if(conn) {
			synchronized (monitorObject) {
				monitorObject.notify();
			}
		}

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
				notifyToast("Thanks!", Toast.LENGTH_SHORT);
			}
		});
		aboutUs.show();
		return;
	}
	
	/* Handle Measure button click */
	public void measure(View viewClick) {
		if(conn) {
			new socketWorker("").execute(measureCmd);
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		
		return;
	}
	
	/* Handle Detail button click */
	public void detailNode(View viewClick) {
		if(conn) {
			new socketWorker("").execute(detailCmd);
			
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		Intent activityIntent = new Intent(this, DetailActivity.class);
		startActivity(activityIntent);
		return;
	}
	
	/* Handle Predict button click */
	public void predict(View viewClick) {
		if(conn) {
			new socketWorker("").execute(predictCmd);
			
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		
		Intent activityIntent = new Intent(this, PredictActivity.class);
		startActivity(activityIntent);
		
		return;		
	}
	
	/* Handle Schedule button click */
	public void schedule(View viewClick) {
		if(conn) {
			new socketWorker("").execute(scheduleCmd);
			Intent activityIntent = new Intent(this, ScheduleActivity.class);
			startActivity(activityIntent);
		}else
			notifyToast("No connection. Please connect again!", Toast.LENGTH_SHORT);
		
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
			/* Open a output stream and a input stream to send and receive data */
	        try{
	        	PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
	        	BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
	            
	            /* Create a frame */
	            DATA = createDataFrame(params[0], data);
	            
	            /* Send frame to server */
	            out.println(DATA);
	            
	            /* Read topology data from server */
	            String line = "";
	            while(true) {
	            	line = in.readLine().toString();
	            	if(line.equalsIgnoreCase("end")) break;
	            	result += line;
	            }
		        
	            return result;
	        }catch(Exception e) {
	        	errno = 1;
	            e.printStackTrace();
	        }
	        
			return null;
		}
		
		@Override
	    protected void onPostExecute(String result) {
			progDialog.dismiss();
			if(errno == 1) { //Transfering error occur.
				notifyToast("Transfering error! Please connect again!", Toast.LENGTH_SHORT);
			}else {
				notifyToast("Done!", Toast.LENGTH_SHORT);
				//TODO
				Log.d("result", result);
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
			clientSocket = new Socket();
			clientSocket.bind(null);
			if(useIpDefault)
				clientSocket.connect((new InetSocketAddress(defaultIP, SERVER_PORT)), 10000);
			else
				clientSocket.connect((new InetSocketAddress(newIP, SERVER_PORT)), 10000);
        }catch(Exception e) {
        	notifyToast("Can not connect!", Toast.LENGTH_SHORT);
            e.printStackTrace();
            
            return false;
        }
		notifyToast("Connected!", Toast.LENGTH_SHORT);
		
		return true;
	}

	/* Toast */
	public void notifyToast(String noice, int timeShow) {
		Toast.makeText(getApplicationContext(), noice, timeShow).show();
		
		return;
	}
	
}
