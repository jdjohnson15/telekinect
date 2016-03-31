#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Change2CUI=true
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
#include <GDIPlus.au3>

main()

Func main()
	Local $image,$clsid
	validate()
	_GDIPlus_Startup ()
	$image = _GDIPlus_ImageLoadFromFile ($CmdLine[1])
	$clsid = _GDIPlus_EncodersGetCLSID (StringRight($CmdLine[2], 3))
	_GDIPlus_ImageSaveToFileEx ($image, $CmdLine[2], $clsid)
	_GDIPlus_ShutDown ()
	ConsoleWrite("Success.")
	Exit
EndFunc   ;==>main

Func validate()
	If $CmdLine[0] = 0 Or $CmdLine[1] = "-help" Then
		ConsoleWrite("supported file types are: bmp,gif,jpg,png,tif")
		ConsoleWrite(@CRLF & "usage: convert fileinput.xxx fileoutput.xxx")
		ConsoleWrite(@CRLF & "note: you must use exact extensions.")
		Exit
	ElseIf $CmdLine[0] > 2 Then
		ConsoleWrite("Error, too many arguments.")
		Exit
	ElseIf $CmdLine[0] < 2 Then
		ConsoleWrite("Error, too few arguments.")
		Exit
	ElseIf StringRight($CmdLine[1], 3) <> "bmp" And StringRight($CmdLine[1], 3) <> "gif" And StringRight($CmdLine[1], 3) <> "jpg" And StringRight($CmdLine[1], 3) <> "png" And StringRight($CmdLine[1], 3) <> "tif" Then
		ConsoleWrite("Error, cannot convert from that file type.")
		Exit
	ElseIf StringRight($CmdLine[2], 3) <> "bmp" And StringRight($CmdLine[2], 3) <> "gif" And StringRight($CmdLine[2], 3) <> "jpg" And StringRight($CmdLine[2], 3) <> "png" And StringRight($CmdLine[2], 3) <> "tif" Then
		ConsoleWrite("Error, cannot convert to that file type.")
		Exit
	ElseIf FileExists($CmdLine[1]) = 0 Then
		ConsoleWrite("Error, cannot find source file.")
		Exit
	EndIf
EndFunc   ;==>validate