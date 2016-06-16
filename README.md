# TeleKinect (Open Telepresence)

This is a Windows application that uses a Kinect v1 to send live data to client apps over the internet for telepresence and augmented reality applications.

The data compression is not optimized, so right now the resulting "video" received by the client is a series of still frames, but it illustrates the proof-of-concept. 

The client app will take the kinect data streamed from a remote or the local machine and render a 3D model of what the kinect sees. When it gets a new frame, it makes a new model. The faster the data transmission, the more animated the model.

## Prerequisities

Server:
* Windows to run pre-compiled exe
* Visual Studio to compile project
* Kinect SDK 1.8
  
Client:
  Unity to compile project for any device that Unity supports. Device must have a camera for augmented reality apps. 
  

## Built With

* Kinect SDK 1.8
* Unity Game Engine
