#load "SendCloudToDeviceMessageAsync.csx"
public static void Run(Stream myBlob, string name, ILogger log){
    StreamReader streamReader = new StreamReader(myBlob);
    String command = streamReader.ReadLine();
    streamReader.Close();
    //根据语音识别结果设置指令msg
    string msg = "";
    if (command == "Open.") msg = "open";
    else if (command == "Close.") msg = "close";
    else msg = "others";
    SendCloudToDeviceMessageAsync(msg).Wait();//向设备发送指令msg
}
