
using UnityEngine;
using System.Collections;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;
using CielaSpike;
using Battlehub.Dispatcher;

// State object for receiving data from remote device.
public class StateObject {
	// Client socket.
	public Socket workSocket = null;
	// Size of receive buffer.
	public const int BufferSize = 2457600;
	// Receive buffer.
	public byte[] buffer = new byte[BufferSize];
	// Received data string.
	public StringBuilder sb = new StringBuilder();
	public List<byte> recvData = new List<byte>();
	public int currByteCount = 0;
	public bool isSettingTexture = false;

	public void Reset(){
		//Debug.Log("resetting");
		currByteCount = 0;
		recvData.Clear();
		isSettingTexture = false;
//		sb.Remove(0,sb.Length-1);
//		Debug.Log("sb length: " + sb.Length);
	}
}


public class socket_client : MonoBehaviour
{

    public Socket sockfd;
    public int portno = 27160;
	public string ip = "100.8.48.145";
    public int distance_range = 8000;
    public IPEndPoint endpoint;
    public int n = 0;
	public byte[] buffer = new byte[2457600];//size of image data
	public byte[] holder = new byte[2457600];//hold buffer data so it can be overwritten 
	public byte[] clientMessage = new byte[1];

    public bool connected = false;

    public int color_mode = 0;

	// ManualResetEvent instances signal completion.
	private static ManualResetEvent connectDone = 
		new ManualResetEvent(false);
	private static ManualResetEvent sendDone = 
		new ManualResetEvent(false);
	private static ManualResetEvent receiveDone = 
		new ManualResetEvent(false);

	StateObject currentState;
	Task clientTask; //thread 1 tracker -> for connection
	Task requestDataTask; //thread 2 tracker -> for send/receive

//	byte[] requestCode = new byte[]{ 0x61 };
    
	void Start()
    {
		currentState = new StateObject();
		this.StartCoroutineAsync(connectToServer(), out clientTask);
        Vector2 pos = new Vector2(0, 0);
        Vector2 scale = new Vector2(1, 1);
        Vector2 rot = new Vector3(180, 0, 0);
        Quaternion rotation = Quaternion.Euler(rot);
        var matrix = Matrix4x4.TRS(pos, rotation, scale);
        GetComponent<Renderer>().material.SetMatrix("_Matrix", matrix);
        //        if (connected)
        //        {
        //            getData();
        //        }
    }

	void OnApplicationQuit() {
		// Release the socket.
		if(clientTask != null) clientTask.Cancel();
		if(requestDataTask != null) requestDataTask.Cancel();
//		if(sockfd!= null){
//			Debug.Log("shutting down socket");
//			sockfd.Shutdown(SocketShutdown.Both);
//			sockfd.Close();
//		}
	}

    void Update()
    {
		//Debug.Log("debugging on main thread");
    }

	IEnumerator connectToServer()
    {
		endpoint = new IPEndPoint(IPAddress.Parse(ip), portno);

        sockfd = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        if (sockfd == null)
            Debug.Log("Error opening socket");

        try
        {
//            sockfd.Connect(endpoint);
			sockfd.BeginConnect(endpoint, new AsyncCallback(ConnectCallback), sockfd);
			connectDone.WaitOne();
            //sockfd.Connect(endpoint);
            Debug.Log("Connected");
//            connected = true;

			Dispatcher.Current.BeginInvoke(() =>
			{
				this.StartCoroutineAsync(RequestData(), out requestDataTask);
			});
//			this.StartCoroutineAsync(RequestData(), out requestDataTask);

        }
        catch (SocketException ex)
        {
            Debug.Log(ex.ToString());
        }

		yield break;

    }

	void ConnectCallback(IAsyncResult ar){
		try {
			// Retrieve the socket from the state object.
			Socket client = (Socket) ar.AsyncState;

			// Complete the connection.
			client.EndConnect(ar);

//			Console.WriteLine("Socket connected to {0}",
//				client.RemoteEndPoint.ToString());
			Debug.Log("socket connected to " + client.RemoteEndPoint.ToString());

			// Signal that the connection has been made.
			connectDone.Set();
		} catch (Exception e) {
			Debug.LogError(e.ToString());
		}
	}

	private IEnumerator Receive(Socket client) {
		while(currentState.isSettingTexture) yield return null;
		try {
			// Create the state object.
//			StateObject currentState = new StateObject();
//			currentState.recvData.Clear();
			currentState.Reset();
			currentState.workSocket = client;
			//Debug.Log("begin receiving from server");
			// Begin receiving the data from the remote device.
			client.BeginReceive(currentState.buffer, 0, StateObject.BufferSize, 0,
				new AsyncCallback(ReceiveCallback), currentState);
		} catch (Exception e) {
			Console.WriteLine(e.ToString());
		}
	}

	private void ReceiveCallback( IAsyncResult ar ) {
		try {
			// Retrieve the state object and the client socket 
			// from the asynchronous state object.
			StateObject state = (StateObject) ar.AsyncState;
			Socket client = state.workSocket;

			// Read data from the remote device.
			int bytesRead = client.EndReceive(ar);

			if (bytesRead > 0) {
				// There might be more data, so store the data received so far.
//				Debug.Log("received " + bytesRead + " bytes from server");
				for(int i=0;i<bytesRead;i++){
					currentState.recvData.Add(currentState.buffer[i]);
				}

//				if (state.recvData[0] != 'n'){
//					assignTexture();
//				}
//				Debug.Log("buffer size: " + buffer.Length);
				state.currByteCount += bytesRead;
					
				if(state.currByteCount == StateObject.BufferSize){
//					Debug.Log("stringbuilder leng: " + state.sb.Length);

					Dispatcher.Current.BeginInvoke(() =>
					{
							//Debug.Log("assigning texture, recvDataCount: " + state.recvData.Count);
							currentState.isSettingTexture = true;
							assignTexture();
					});

					receiveDone.Set();
				}
				else{
					// Get the rest of the data.
					client.BeginReceive(state.buffer,0,StateObject.BufferSize,0,
						new AsyncCallback(ReceiveCallback), state);
				}
			} else {
				// All the data has arrived; put it in response.

				//Debug.Log("No data are being read");

				if(state.currByteCount == StateObject.BufferSize){
					//					Debug.Log("stringbuilder leng: " + state.sb.Length);

					Dispatcher.Current.BeginInvoke(() =>
						{
							//Debug.Log("assigning texture, recvDataCount: " + state.recvData.Count);
							currentState.isSettingTexture = true;
							assignTexture();
						});
				}
				receiveDone.Set();

				// Signal that all bytes have been received.
//				receiveDone.Set();
			}
		} catch (Exception e) {
			Debug.LogError(e.ToString());
		}
	}

//	private static void Send(Socket client, String data) {
//		// Convert the string data to byte data using ASCII encoding.
//		byte[] byteData = Encoding.ASCII.GetBytes(data);
//
//		// Begin sending the data to the remote device.
//		client.BeginSend(byteData, 0, byteData.Length, 0,
//			new AsyncCallback(SendCallback), client);
//	}

	private void SendRequest(Socket client) {
		// Begin sending the data to the remote device.
		byte[] requestCode = { 0x61 };
		//Debug.Log("beginning to send data");
		client.BeginSend(requestCode, 0, requestCode.Length, 0,
			new AsyncCallback(SendCallback), client);
	}

	private void SendCallback(IAsyncResult ar) {
		try {
			// Retrieve the socket from the state object.
			Socket client = (Socket) ar.AsyncState;

			// Complete sending the data to the remote device.
			int bytesSent = client.EndSend(ar);
//			Console.WriteLine("Sent {0} bytes to server.", bytesSent);
			//Debug.Log("Sent " + bytesSent + " bytes to server");

			// Signal that all bytes have been sent.
			sendDone.Set();
		} catch (Exception e) {
			Debug.LogError(e.ToString());
		}
	}

	//recurive function that always tries to request data
	IEnumerator RequestData(){
		SendRequest(sockfd);
		sendDone.WaitOne();

		yield return Receive(sockfd);
		receiveDone.WaitOne();
		// Write the response to the console.
		//Debug.Log("Response received");

		sendDone.Reset();
		receiveDone.Reset();

//		this.StartCoroutineAsync(RequestData(), out requestDataTask);
		Dispatcher.Current.BeginInvoke(() =>
			{
				this.StartCoroutineAsync(RequestData(), out requestDataTask);
			});
	}

//    void getData()
//    {
//        try
//        {
//            byte[] bytes = { 0x61 };
//            // Send message to server
//            sockfd.Send(bytes, bytes.Length, SocketFlags.None);
//
//            try
//            {
//                // Receive message from server
//				int foo = sockfd.Receive(buffer,2457600, SocketFlags.None);
//                Debug.Log("received: "+foo);
//				if (buffer[0] != 'n'){
//                    assignTexture();
//				}
//            }
//            catch (SocketException ex)
//            {
//                Debug.Log("Error Reading From Socket: " + ex.ToString());
//            }
//        }
//        catch (SocketException ex)
//        {
//            Debug.Log("Error Writing To Socket: " + ex.ToString());
//        }
//    }
//
    static byte[] GetBytes(string str)
    {
        return System.Text.Encoding.UTF8.GetBytes(str);
    }
//
//    static string GetString(byte[] bytes)
//    {
//        return System.Text.Encoding.UTF8.GetString(bytes, 0, bytes.Length);
//    }

	void assignTexture()
    {

		Texture2D tex = new Texture2D(640, 960, TextureFormat.RGBA32, false);
        // Load data into the texture and upload it to the GPU.
//        tex.LoadRawTextureData(buffer);
		tex.LoadRawTextureData(currentState.recvData.ToArray());
//		tex.LoadRawTextureData(GetBytes(currentState.sb.ToString()));
//		tex.LoadRawTextureData(currentState.buffer);
		tex.Apply();
        // Assign texture to renderer's material.
        GetComponent<Renderer>().material.mainTexture = tex;
		currentState.isSettingTexture = false;
        setRange();
        setMode();
    }

    void setRange()
    {
        GetComponent<Renderer>().material.SetFloat("_Range", distance_range);
    }
    void setMode()
    {
        GetComponent<Renderer>().material.SetInt("_Mode", color_mode);
    }

}
