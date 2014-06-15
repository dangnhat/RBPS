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
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.text.Html;
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
//	private final int frameSize = 257;
	public Socket clientSocket;
//	public String FRAME[] = new String[frameSize];
	public static String FRAME;
	private final int SERVER_PORT = 9999;
	private final String SERVER_IP_DEFAULT = "192.168.150.1";
	private static String SERVER_IP = "";
//	private PrintWriter out = null;
//	private BufferedReader in = null;
	private final String scanAct = "1";
	private final String measureAct = "2";
	private final String detailAct = "3";
	private final String databaseAct = "4";
	private static boolean useIpDefault = true;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
//		String line = "1234";
//		line = line.substring(0, 1); 
//		TextView detailView = (TextView)findViewById(R.id.viewDetails);
//		detailView.setText(line);
		Button measureBtn = (Button)findViewById(R.id.btnMeasure);
		Button detailBtn = (Button)findViewById(R.id.btnDetail);
		/* If have no node, disable the Measure and Detail button */
		measureBtn.setClickable(false);
		detailBtn.setClickable(false);
		
		ListView topoView = (ListView)findViewById(R.id.viewTopo);
		ArrayList<String> list = new ArrayList<String>();
		list.add("node 1");
		list.add("node 2");
		list.add("node 3");
		list.add("node 4");
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
					android.R.layout.simple_expandable_list_item_1, list);
		topoView.setAdapter(adapter);
		topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent,
					View view, int position, long id) {
				// TODO Auto-generated method stub
				view.setBackgroundColor(Color.BLUE);
			}
		});
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
	    case R.id.action_database:
	    	viewDatabase();
	    	return true;
	    case R.id.action_schedule:
            schedule();
            return true;	
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
	
	public void viewDatabase() {
		if(useIpDefault) {
			new socketWorker().execute(SERVER_IP_DEFAULT, detailAct);
		}else {
			new socketWorker().execute(SERVER_IP, detailAct);
		}
		
		return;
	}
	
	public void schedule() {
		
		return;
	}
	
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
	
	/* Handle Scan button click */
	public void scanTopo(View viewClick) {
		if(useIpDefault) {
			new socketWorker().execute(SERVER_IP_DEFAULT, scanAct);
		}else {
			new socketWorker().execute(SERVER_IP, scanAct);
		}
		
		return;
	}
	
	/* Handle Measure button click */
	public void measure(View viewClick) {
		if(useIpDefault) {
			new socketWorker().execute(SERVER_IP_DEFAULT, measureAct);
		}else {
			new socketWorker().execute(SERVER_IP, measureAct);
		}
		
		return;
	}
	
	/* Handle Detail button click */
	public void detailNode(View viewClick) {
		if(useIpDefault) {
			new socketWorker().execute(SERVER_IP_DEFAULT, detailAct);
		}else {
			new socketWorker().execute(SERVER_IP, detailAct);
		}
		
		return;
	}
		
	/* Process transceiver */
	public class socketWorker extends AsyncTask<String, Void, String> {
		private int errno = 0;
		private TextView detailView = (TextView)findViewById(R.id.viewDetails);
		private ListView topoView = (ListView)findViewById(R.id.viewTopo);
		private Button scanBtn = (Button)findViewById(R.id.btnScan);
		private Button measureBtn = (Button)findViewById(R.id.btnMeasure);
		private Button detailBtn = (Button)findViewById(R.id.btnDetail);
    	private ProgressDialog progDialog;
    	
    	String result = "";
    	
		@Override
		protected void onPreExecute() {
			detailView.setText("abc");
			
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
	    	scanBtn.setClickable(false);
	    	measureBtn.setClickable(false);
	    	detailBtn.setClickable(false);
	    	
			/* If code segment below be reached, starting transceiver data */
	        try{
	        	PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
	        	BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
	        	
	        	String data = "";	//data in a frame.
	            
	            if(params[1].equals(scanAct)) {
	            	data = "";
	            }else if(params[1].equals(measureAct)) {
	            	data = "12";	//patientID
	            }else if(params[1].equals(detailAct)) {
	            	data = "12";	//patientID
	            }else if(params[1].equals(databaseAct)) {
	            	data = "";
	            }
	            
	            /* Create a frame */
	            FRAME = createFrame(params[1], data);
	            
	            result += params[1];
	            
	            /* Send frame to server */
	            out.println(FRAME);
	            
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
		        scanBtn.setClickable(true);
//		        measureBtn.setClickable(true);
//		        detailBtn.setClickable(true);
		        
	            return result;
	        }catch(Exception e) {
	        	errno = 2;
	        	
	        	/* Enable all buttons */
		        scanBtn.setClickable(true);
//		        measureBtn.setClickable(true);
//		        detailBtn.setClickable(true);
		        
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
				String cmd = result.substring(0, 1); //get cmd.
				if(cmd.equals(scanAct)) {
					ArrayList<String> list = new ArrayList<String>();
					ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
								android.R.layout.simple_list_item_1, list);
					topoView.setAdapter(adapter);
					topoView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

						@Override
						public void onItemClick(AdapterView<?> parent,
								View view, int position, long id) {
							// TODO Auto-generated method stub
							
						}
					});
				}else if(cmd.equals(measureAct)) {
					
				}else if(cmd.equals(detailAct)) {
					
				}else if(cmd.equals(databaseAct)) {
					
				}
			}
	    }
		
	}
	
	public String createFrame(String cmd, String data) {
		String frame = "";
		int lengthData = data.length();

		frame += String.valueOf(lengthData) + cmd + data;
		
		return frame;
	}

}
