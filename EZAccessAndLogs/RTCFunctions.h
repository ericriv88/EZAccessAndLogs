////////////////////////////////////////////////////////////////////////////
//***********************RTC Functions***********************************///
////////////////////////////////////////////////////////////////////////////

String DateandTime(RTCZero rtc)  //returns the date and time in string format
{
  String ret = "";
  ret = String(rtc.getMonth()) + "/" + String(rtc.getDay()) + "/" + String(rtc.getYear()) + " ";
  int hours = rtc.getHours() - 7;      //convert hours to PST
  if(hours < 0) hours = hours + 24;   //if hour happens to be negative bring it back to actual value
  if(hours < 10) {
    ret += "0";
  }
  ret += String(hours) + ":";
  if(rtc.getMinutes() < 10) {
    ret += "0";
  }
  ret += String(rtc.getMinutes()) + ":";
  if(rtc.getSeconds() < 10) {
    ret += "0";
  }
  ret += String(rtc.getSeconds());
  return ret;
}