
using UnityEngine;
using System.Collections;
 
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

public class socket_client_Jess : MonoBehaviour
{

    public Socket sockfd;
    public int portno = 27160;
	public string ip = "100.8.48.145";
    public IPEndPoint endpoint;
	static public int bufLen = 2457600;
    public int n = 0;
	public byte[] buffer = new byte[bufLen];//size of image data
	public byte[] holder = new byte[bufLen];//hold buffer data so it can be overwritten 
	public int totalBytes = 0; //ensures whole buffer is read
	public byte[] clientMessage = new byte[1];

    public bool connected = false;

    void Start()
    {
        connectToServer();
        if (connected)
        {
            getData();
        }
    }

    void Update()
    {
        if (connected)
        {
           // if(Input.GetKeyUp(KeyCode.A))
            //{
              //  getData();
           // }
           
            
            getData();
            
        }
        else
        {
            connectToServer();
        }
    }

    void connectToServer()
    {
		endpoint = new IPEndPoint(IPAddress.Parse(ip), portno);

        sockfd = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        if (sockfd == null)
            Debug.Log("Error opening socket");

        try
        {
            sockfd.Connect(endpoint);
            //sockfd.Connect(endpoint);
            Debug.Log("Connected");
            connected = true;

        }
        catch (SocketException ex)
        {
            Debug.Log(ex.ToString());
        }
    }

    void getData()
    {
        try
        {
            byte[] bytes = { 0x61 };
            // Send message to server
			//Debug.Log("sending...");
            sockfd.Send(bytes, bytes.Length, SocketFlags.None);

            try
            {
                // Receive message from server
				/*while (totalBytes < bufLen){
					iResult = sockfd.Receive(holder, 0, bufLen - 0, SocketFlags.None);
					Debug.Log("iResult: "+iResult);
					for (int i = 0; i < iResult; ++i){
						buffer[i+ totalBytes] = holder[i];
						Debug.Log(i);
					}
					totalBytes += iResult;

					Debug.Log("received: "+totalBytes);
				//}*/

				int iResult = sockfd.Receive(buffer, totalBytes, bufLen - totalBytes, SocketFlags.None);
				//Debug.Log("iResult: " + iResult);

				//Debug.Log("end total received: " + totalBytes);
				if (buffer[totalBytes] != 'n'){
					totalBytes+= iResult;/*
					int t = totalBytes%4;
					if (t != 0)
						totalBytes -= t;	*/
					if (totalBytes == bufLen)
						totalBytes = 0;
                    assignTexture();
				}
				else
					Debug.Log("not ready");
            }
            catch (SocketException ex)
            {
                Debug.Log("Error Reading From Socket: " + ex.ToString());
            }
        }
        catch (SocketException ex)
        {
            Debug.Log("Error Writing To Socket: " + ex.ToString());
        }
    }

    static byte[] GetBytes(string str)
    {
        return System.Text.Encoding.UTF8.GetBytes(str);
    }

    static string GetString(byte[] bytes)
    {
        return System.Text.Encoding.UTF8.GetString(bytes, 0, bytes.Length);
    }

    void assignTexture()
    {

		Texture2D tex = new Texture2D(640, 960, TextureFormat.RGBA32, false);
        // Load data into the texture and upload it to the GPU.
        byte[] buf = new byte[2457600];
        rotate180(buf);
        tex.LoadRawTextureData(buf);
        tex.Apply();
        // Assign texture to renderer's material.
        GetComponent<Renderer>().material.mainTexture = tex;
    }
    void rotate180(byte[] buf)
    {
        for (int i = 0; i < 2457600; i += 4)
        {
            buf[i] = buffer[2457600 - i * 4];
            buf[i + 1] = buffer[2457600 - i * 4 + 1];
            buf[i + 2] = buffer[2457600 - i * 4 + 2];
            buf[i + 3] = buffer[2457600 - i * 4 + 3];
        }
    }
}
