using UnityEngine;
using System.Collections;
using System;
using System.Net;
using System.Net.Sockets;
using System.IO;

public class TCPClient : MonoBehaviour {

	/*
	TcpClient mySocket;
	NetworkStream theStream;
	StreamWriter theWriter;
	StreamReader theReader;
	bool socketReady = false;

	// Use this for initialization
	void Start () {
		setupSocket();
	}
	
	// Update is called once per frame
	void Update () {
		readSocket();
	}
	
	public void setupSocket()
	{
		try
		{
			mySocket = new TcpClient("127.0.0.1", 27015);
			theStream = mySocket.GetStream();
			theWriter = new StreamWriter(theStream);
			theReader = new StreamReader(theStream);
			socketReady = true;
			print("Connection success");
		}
		catch (Exception e)
		{
			Debug.Log("Socket error: " + e);
			print("Connection failed");
		}
	}
	
	public void readSocket()
	{
		if (!socketReady)
			print ("Connection failed: Tried to read");
		
		if (theStream.DataAvailable)
			print(theReader.ReadLine());
		
	}
	*/
	private Socket sockfd;
	int portno, n;
	byte[] buffer = new byte[256];
	public bool sendMessage = false;

	void OnValidate(){
		if(sendMessage == true){
			try {
				byte[] bytes = GetBytes("sendMeData");
				// Send message to server
				sockfd.Send(bytes, bytes.Length, SocketFlags.None);

				try {
					// Receive message from server
					sockfd.Receive(buffer);
					Debug.Log (GetString (buffer));
				}
				catch (SocketException ex) {
					Debug.Log("Error Reading From Socket: " + ex.ToString ());
				}
			}
			catch (SocketException ex) {
				Debug.Log("Error Writing To Socket: " + ex.ToString ());
			}
			sendMessage = false;
		}

	}

	void Start() {
		portno = 27160;
		sockfd = new Socket (AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
		if (sockfd == null)
			Debug.Log ("Error opening socket");

		try {
//			StartCoroutine(RequestData());
			sockfd.Connect (IPAddress.Loopback, portno);
			Debug.Log ("Connected");
			try {
								byte[] bytes = GetBytes ("Hello from Parth");
//				byte[] bytes = GetBytes("sendMeData");
				// Send message to server
				sockfd.Send(bytes, bytes.Length, SocketFlags.None);

				try {
					// Receive message from server
					sockfd.Receive(buffer);
					Debug.Log (GetString (buffer));
				}
				catch (SocketException ex) {
					Debug.Log("Error Reading From Socket: " + ex.ToString ());
				}
			}
			catch (SocketException ex) {
				Debug.Log("Error Writing To Socket: " + ex.ToString ());
			}
		}
		catch (SocketException ex) {
			Debug.Log(ex.ToString());
		}
	}

	static byte[] GetBytes(string str)
	{
//		byte[] bytes = new byte[str.Length * sizeof(char)];
//		System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
//		return bytes;
		return System.Text.Encoding.UTF8.GetBytes(str); //Text need to be in UTF8 format to work properly on both ends!
	}

	static string GetString(byte[] bytes)
	{
//		char[] chars = new char[bytes.Length / sizeof(char)];
//		System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
//		Debug.Log(chars.Length);
//		return new String(chars);
		return System.Text.Encoding.UTF8.GetString(bytes, 0, bytes.Length);
	}

//	IEnumerator RequestData(){
//		sockfd.Connect (IPAddress.Loopback, portno);
//		Debug.Log ("Connected");
//		while(true){
//			try {
////				byte[] bytes = GetBytes ("Hello from Parth");
//				byte[] bytes = GetBytes("sendMeData");
//				// Send message to server
//				sockfd.Send(bytes, bytes.Length, SocketFlags.None);
//
//				try {
//					// Receive message from server
//					sockfd.Receive(buffer);
//					Debug.Log (GetString (buffer));
//				}
//				catch (SocketException ex) {
//					Debug.Log("Error Reading From Socket: " + ex.ToString ());
//					yield break;
//				}
//			}
//			catch (SocketException ex) {
//				Debug.Log("Error Writing To Socket: " + ex.ToString ());
//				yield break;
//			}
//			yield return null;
//		}
//	}

	/*
	private Socket _clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
	private byte[] _recieveBuffer = new byte[256];

	void Start() {
		SetupServer ();
	}

	private void SetupServer()
	{
		try
		{
			//_clientSocket.Connect(new IPAddress(0xD8A55F03), 27015);
			_clientSocket.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"),27015));
		}
		catch(SocketException ex)
		{
			Debug.Log(ex.Message);
		}
		
		_clientSocket.BeginReceive(_recieveBuffer,0,_recieveBuffer.Length,SocketFlags.None,new AsyncCallback(ReceiveCallback),null);
		
	}
	
	private void ReceiveCallback(IAsyncResult AR)
	{
		//Check how much bytes are recieved and call EndRecieve to finalize handshake
		int recieved = _clientSocket.EndReceive(AR);
		
		if(recieved <= 0)
			return;
		
		//Copy the recieved data into new buffer , to avoid null bytes
		byte[] recData = new byte[recieved];
		Buffer.BlockCopy(_recieveBuffer,0,recData,0,recieved);
		
		//Process data here the way you want , all your bytes will be stored in recData
		
		//Start receiving again
		_clientSocket.BeginReceive(_recieveBuffer,0,_recieveBuffer.Length,SocketFlags.None,new AsyncCallback(ReceiveCallback),null);
	}
	
	private void SendData(byte[] data)
	{
		SocketAsyncEventArgs socketAsyncData = new SocketAsyncEventArgs();
		socketAsyncData.SetBuffer(data,0,data.Length);
		_clientSocket.SendAsync(socketAsyncData);
	}
	*/
}


