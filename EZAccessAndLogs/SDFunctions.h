////////////////////////////////////////////////////////////////////////////
//************************SD Functions***********************************///
////////////////////////////////////////////////////////////////////////////

#include "HashFunctions.h"

bool readSDBool(String fileName) {
  File myFile;                        //file varriable for SD card use
  myFile = SD.open(fileName);        //opens .txt file specified by fileName
  String temp = "";
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(temp == "true") {  //if string is "true" then close file and return true
        myFile.close();
        return true;
      }
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file and return false
  return false; 
}

void overWriteSDBool(String fileName, bool set) {
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file fr overwrite
  myFile = SD.open(fileName, FILE_WRITE);
  //write true or false to file depending on set
  if(set) { 
    myFile.println("true");
  }
  else {
    myFile.println("false");
  }
  myFile.close();
}

String readSDLine(String fileName, int line) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  int lineCount = 1;
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(lineCount == line) {   //if on correct line, close file and return temp
        myFile.close();
        return temp;
      }
      temp = "";  //clear temp for next line
      lineCount += 1; //increment lineCount
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  return temp;      //return the empty string if defined line is empty
}

int SDLineCount(String fileName) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  int lineCount = 0;          //initialize linecount
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      lineCount += 1; //increment lineCount
    }
  }
  myFile.close();   //close file
  return lineCount;      //retrn line ocunt
}

void overWriteSD(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file for overwrite
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(content);
  myFile.close();
}

void overWriteSDHash(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file for overwrite
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(toHash(content));
  myFile.close();
}

void wipeSDFile(String fileName) {
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file 
  myFile = SD.open(fileName, FILE_WRITE);   //recreate file
  myFile.close();       //close file
}

void deleteSDLine(String fileName, int line) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  String SDret;            //initialize string that will be written to SD
  int lineCount = 1;
  if(SDLineCount(fileName) == 1) {  //if there is only one line wipefile
    myFile.close();   //close file
    wipeSDFile(fileName);
    return;
  }
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(lineCount == line) {   //if on line to delete, dont add line to return string
        temp = "";  //clear temp for next line
        lineCount += 1; //increment lineCount
      }
      else {
        SDret += temp;    //add line to return string with carage return
        temp = "";  //clear temp for next line
        if(line == SDLineCount(fileName) && lineCount == SDLineCount(fileName) -1) {    //gets rid of extra line when deleting last line
          //dont add any end of line
        }
        else if(lineCount != SDLineCount(fileName)) {   //add end of line unless on last line 
          SDret += '\r';
          SDret += '\n';
        }
        lineCount += 1; //increment lineCount
      }
      
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  overWriteSD(fileName, SDret); //overwrite file with new string that has deleted the line
}

bool checkSDForString(String fileName, String givenString) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(temp == givenString) {   //if line on SD equals the given string, close file and return true
        myFile.close();
        return true;
      }
      temp = "";  //clear temp for next line
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  return false;      //retuern the empty string if defined line is empty
}

int findSDStringLine(String fileName, String givenString) {
  if(! checkSDForString(fileName, givenString)) return 0;  //return 0 if string is not found
  //if string is found find the line number
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  int line = 1;
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(temp == givenString) {   //if the string is found, return its line
        myFile.close();
        return line;
      }
      temp = "";  //clear temp for next line
      line++;     //increment line
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();     //close file just incase
}

void writeSDLine(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(content);
  myFile.close();
}

void writeSDHashLine(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(toHash(content));
  myFile.close();
}

