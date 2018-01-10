/** @file SchedulerControl.C
*  @brief Application for creating and listing schedules
*  @author Weija Zhou
*  @date Nov 28, 2017
*/

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>
#include <Wt/WSlider>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/WCalendar>
#include <Wt/WTimeEdit>
#include <Wt/WTime>
#include <Wt/WDate>
#include <Wt/WComboBox>
#include <Wt/Http/Client>
#include <string>
#include "SingleSchedulerControl.h"
#include "Session.h"
#include <unistd.h>



using namespace Wt;
using namespace std; 

SingleSchedulerControlWidget::SingleSchedulerControlWidget(Session *session, WContainerWidget *parent):
  WContainerWidget(parent),
  session_(session)
{
  setContentAlignment(AlignCenter);
  setStyleClass("highscores");
}

void SingleSchedulerControlWidget::update()
{
  clear();

  Datalight = '0';

  Datahour = "01"; 
  Datamin = "00";
  Datasec = "00"; 

   deleteConfirm = false; 
  //get URL info
  string address = WApplication::instance()->internalPath();

  size_t pos = address.find("user=");           //get userID
  string subString = address.substr(pos + 5);
  size_t endPos = subString.find("&");
  userID = subString.substr(0, endPos);
  pos = address.find("ip=");                //get ip
  subString = address.substr(pos + 3);
  endPos = subString.find("&");
  ip = subString.substr(0, endPos);
  pos = address.find("port=");                //get port
  subString = address.substr(pos + 5);
  endPos = subString.find("&");
  port = subString.substr(0, endPos);
  pos = address.find("scheduleid=");                 //get scheduleid
  subString = address.substr(pos + 11);
  endPos = subString.find("&");
  scheduleID = subString.substr(0, endPos);

  pos = address.find("name=");                 //get scheduleid
  subString = address.substr(pos + 5);
  endPos = subString.find("&");
  nameID = subString.substr(0, endPos);

  //display user info in top left corner
  string firstName = session_->firstName();
  string lastName = session_->lastName();
  WText *userInfo_ = new WText(this);
  userInfo_->setTextAlignment(AlignmentFlag::AlignLeft);
  userInfo_->setText("Hello, " + firstName + " " + lastName);
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //get group info to display 
  if (scheduleID != "99"){

    Http::Client *client = SingleSchedulerControlWidget::connect();
    client->done().connect(boost::bind(&SingleSchedulerControlWidget::handleHttpResponseName, this, _1, _2));
    if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/schedules/"+scheduleID)) {
      WApplication::instance()->deferRendering();
    }
  }
  

  scheduleInfoEdit_ = new WText(this);               //Schedule name
  this->addWidget(new WBreak());
  scheduleTimeEdit_ = new WText(this);               //Time of Schedule 
  this->addWidget(new WBreak());

  this->addWidget(new WText("Select the light to be changed: "));
  this->addWidget(new WBreak());
  WPushButton *oneButton  = new WPushButton("Light 1", this);                   // 1st light button
  oneButton->setMargin(10, Left);
  oneLight_ = new WText(this);
  this->addWidget(new WBreak());
  WPushButton *twoButton = new WPushButton("Light 2", this);          //2nd light button
  twoButton->setMargin(10, Left);
  twoLight_ = new WText(this);
  this->addWidget(new WBreak());
  WPushButton *threeButton = new WPushButton("Light 3", this);                   // 3rd light button
  threeButton->setMargin(10, Left);
  threeLight_ = new WText(this);
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());                        
  this->addWidget(new WBreak());

  //turn on
  this->addWidget(new WText("Light on/off: "));
  WPushButton *onButton
    = new WPushButton("ON", this);                      // ON button
  onButton->setMargin(5, Left);                         

  //turn off
  WPushButton *offButton
    = new WPushButton("OFF", this);                     // OFF button
  offButton->setMargin(5, Left);                         
  this->addWidget(new WBreak());                     
  this->addWidget(new WBreak());
  //change hue
  this->addWidget(new WText("Hue: "));
  this->addWidget(new WBreak());
  this->addWidget(new WText("0  "));
  hueScaleSlider_ = new WSlider(this);           //slider bar
  hueScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
  hueScaleSlider_->setMinimum(0);
  hueScaleSlider_->setMaximum(65535);
  hueScaleSlider_->setValue(100);
  hueScaleSlider_->setTickInterval(10000);
  hueScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
  hueScaleSlider_->resize(300, 50);
  this->addWidget(new WText("  65535"));
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //change brightness
  this->addWidget(new WText("Brightness: "));
  this->addWidget(new WBreak());
  this->addWidget(new WText("1  ")); 
  briScaleSlider_ = new WSlider(this);           //slider bar
  briScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
  briScaleSlider_->setMinimum(1);
  briScaleSlider_->setMaximum(254);
  briScaleSlider_->setValue(100);
  briScaleSlider_->setTickInterval(50);
  briScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
  briScaleSlider_->resize(300, 50);
  this->addWidget(new WText("  254"));
  this->addWidget(new WBreak());                       
  this->addWidget(new WBreak());

  //change saturation
  this->addWidget(new WText("Saturation: "));
  this->addWidget(new WBreak());
  this->addWidget(new WText("0  "));
  satScaleSlider_ = new WSlider(this);          //slider bar
  satScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
  satScaleSlider_->setMinimum(0);
  satScaleSlider_->setMaximum(254);
  satScaleSlider_->setValue(100);
  satScaleSlider_->setTickInterval(50);
  satScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
  satScaleSlider_->resize(300, 50);
  this->addWidget(new WText("  254"));
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //change transition time
  this->addWidget(new WText("Transition Time: (multiple of 100ms) "));
  this->addWidget(new WBreak());
  this->addWidget(new WText("1  (100ms)"));
  transitionScaleSlider_ = new WSlider(this);          //slider bar
  transitionScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
  transitionScaleSlider_->setMinimum(1);
  transitionScaleSlider_->setMaximum(20);
  transitionScaleSlider_->setValue(4);
  transitionScaleSlider_->setTickInterval(2);
  transitionScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
  transitionScaleSlider_->resize(300, 50);
  this->addWidget(new WText("  20 (2 seconds)"));

  this->addWidget(new WBreak());                       
  this->addWidget(new WBreak());
  light_ = new WText(this);                           // displays which light is being changed
  this->addWidget(new WBreak());
  change_ = new WText(this);                          //displays the status of a light change
  this->addWidget(new WBreak());
  
  this->addWidget(new WText("Scheduler")); 

  this->addWidget(new WBreak());
  hourInput_ = new WComboBox(this);
   for (int i = 1; i<13; i++){
      if (i<10){
        hourInput_->addItem("0"+to_string(i));
      }
      else{
        hourInput_->addItem(to_string(i));
      }
      
  }
  this->addWidget(new WText(":")); 
  minInput_ = new WComboBox(this);
  for (int i = 0; i<60; i++){
      if (i<10){
        minInput_->addItem("0"+to_string(i));
      }
      else{
        minInput_->addItem(to_string(i));
      }
  }
  this->addWidget(new WText(":")); 
  secInput_ = new WComboBox(this);
  for (int i = 0; i<60; i++){
      if (i<10){
        secInput_->addItem("0"+to_string(i));
      }
      else{
        secInput_->addItem(to_string(i));
      }
  }
  amSelector_ = new WComboBox(this);
  amSelector_->addItem("AM");
  amSelector_->addItem("PM");

  this->addWidget(new WBreak());
  this->addWidget(new WText("For how many days do you want the schedule to repeat?"));
  reoccurChoices_ = new WComboBox(this);
  reoccurChoices_->addItem("0");
  reoccurChoices_->addItem("1");
  reoccurChoices_->addItem("2");
  reoccurChoices_->addItem("3");
  reoccurChoices_->addItem("4");
  reoccurChoices_->addItem("5");
  reoccurChoices_->addItem("6");
  reoccurChoices_->addItem("7");
  
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  dateSelect_ = new WText(this);
  dateSelect_->setText("Selected Date:          ");
  this->addWidget(new WBreak());
  calendar_ = new WCalendar(this);
  calendar_->setSingleClickSelect(true);
  this->addWidget(new WBreak());



if (scheduleID != "99"){
  scheduleButton = new WPushButton("Edit Schedule", this);   
  deleteButton   = new WPushButton("Delete This Schedule", this);                
  onButton->setMargin(5, Left);
}
else{
  scheduleButton = new WPushButton("Create Schedule", this);                      
  onButton->setMargin(5, Left);
}  


  WPushButton *groupButton            
    = new WPushButton("Go to My Schedules", this);
  groupButton->setLink("/?_=/scheduler?user=" + userID + "%26ip=" + ip + "%26port=" + port);
  groupButton->setMargin(10, Left);


  WPushButton *returnButton             //go back to bridge
    = new WPushButton("Return To Bridge", this);

  onButton->clicked().connect(this, &SingleSchedulerControlWidget::on);
  offButton->clicked().connect(this, &SingleSchedulerControlWidget::off);
  oneButton->clicked().connect(this, &SingleSchedulerControlWidget::lightOne);
  twoButton->clicked().connect(this, &SingleSchedulerControlWidget::lightTwo);
  threeButton->clicked().connect(this, &SingleSchedulerControlWidget::lightThree);
  returnButton->clicked().connect(this, &SingleSchedulerControlWidget::returnBridge);
  briScaleSlider_->valueChanged().connect(this, &SingleSchedulerControlWidget::bright);
  satScaleSlider_->valueChanged().connect(this, &SingleSchedulerControlWidget::sat);
  hueScaleSlider_->valueChanged().connect(this, &SingleSchedulerControlWidget::hue);
  transitionScaleSlider_->valueChanged().connect(this, &SingleSchedulerControlWidget::transition);
  calendar_->clicked().connect(this, &SingleSchedulerControlWidget::changeDate);
  hourInput_->changed().connect(this, &SingleSchedulerControlWidget::changeHour);
  minInput_->changed().connect(this, &SingleSchedulerControlWidget::changeMin);
  secInput_->changed().connect(this, &SingleSchedulerControlWidget::changeSec);
  if (scheduleID != "99"){
  deleteButton->clicked().connect(this, &SingleSchedulerControlWidget::deleteSchedule);
}
  scheduleButton->clicked().connect(this, &SingleSchedulerControlWidget::createSchedule);




  (boost::bind(&SingleSchedulerControlWidget::transition, this));
  (boost::bind(&SingleSchedulerControlWidget::hue, this));
  (boost::bind(&SingleSchedulerControlWidget::bright, this));
  (boost::bind(&SingleSchedulerControlWidget::sat, this));
  (boost::bind(&SingleSchedulerControlWidget::on, this));
  (boost::bind(&SingleSchedulerControlWidget::off, this));
  (boost::bind(&SingleSchedulerControlWidget::handleHttpResponse, this));
  (boost::bind(&SingleSchedulerControlWidget::handleHttpResponseVOID, this));
  (boost::bind(&SingleSchedulerControlWidget::connect, this));
  (boost::bind(&SingleSchedulerControlWidget::lightOne, this));
  (boost::bind(&SingleSchedulerControlWidget::lightTwo, this));
  (boost::bind(&SingleSchedulerControlWidget::lightThree, this));
  (boost::bind(&SingleSchedulerControlWidget::returnBridge, this));
  (boost::bind(&SingleSchedulerControlWidget::changeDate, this));
  (boost::bind(&SingleSchedulerControlWidget::changeHour, this));
  (boost::bind(&SingleSchedulerControlWidget::changeMin, this));
  (boost::bind(&SingleSchedulerControlWidget::changeSec, this));
  (boost::bind(&SingleSchedulerControlWidget::createSchedule, this));
   if (scheduleID != "99"){
  (boost::bind(&SingleSchedulerControlWidget::deleteSchedule, this));
}
  




  Wt::log("info") << "Check 32";

}

//creates a client
Http::Client * SingleSchedulerControlWidget::connect() {
  Http::Client *client = new Http::Client(this);
  client->setTimeout(15);
  client->setMaximumResponseSize(10 * 1024);
}

//handle request (does nothing withthe response) - for changing the light state
void SingleSchedulerControlWidget::handleHttpResponseName(boost::system::error_code err, const Http::Message& response) {
  WApplication::instance()->resumeRendering();
  if (!err && response.status() == 200) {
    //get group name and lights in the group
    Json::Object result;
    Json::parse(response.body(), result);
    string name = result.get("name");
    string time = result.get("time"); 
    
    scheduleInfoEdit_->setText("Schedule Name: " + name);
    scheduleTimeEdit_->setText("Time Of Schedule " + time);

  }
}

//handle request (does nothing withthe response) - for changing the light state
void SingleSchedulerControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

//handles get lights request
void SingleSchedulerControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
  WApplication::instance()->resumeRendering();
  if (!err && response.status() == 200) {
    Json::Object result;
    Json::parse(response.body(), result);

    //get sat
    size_t pos = response.body().find("sat");
    string subString = response.body().substr(pos + 5);
    size_t endPos = subString.find(",");
    string sat = subString.substr(0, endPos);

    //get bri
    pos = response.body().find("bri");
    subString = response.body().substr(pos + 5);
    endPos = subString.find(",");
    string bri = subString.substr(0, endPos);

    //get hue
    pos = response.body().find("hue");
    subString = response.body().substr(pos + 5);
    endPos = subString.find(",");
    string hue = subString.substr(0, endPos);

    hueScaleSlider_->setValue(stoi(hue));
    satScaleSlider_->setValue(stoi(sat));
    briScaleSlider_->setValue(stoi(bri));
  }
}

void SingleSchedulerControlWidget::changeDate() {
  std::set<Wt::WDate> selection = (calendar_->selection());
  WDate date = (*selection.begin());
  
  Datayear = date.year(); 
  Datamonth = date.month(); 
  Dataday = date.day(); 
  std::string selectYear = to_string(Datayear);
  std::string selectMonth = to_string(Datamonth); 
  std::string selectDay = to_string(Dataday); 
  std::string output = "Selected Date:" + selectYear; 
  //Formats output string
  if (Datamonth<10){
   output +="/0"+selectMonth;
  }
  else{
    output += "/"+selectMonth;
  }
  if (Dataday<10){
   output +="/0"+selectDay;
  }
  else{
    output +="/"+selectDay;
  }
  dateSelect_->setText(output); 
  

}
//selects light 1 to change
void SingleSchedulerControlWidget::lightOne() {
  Datalight = '1';
  light_->setText("You are changing Light 1     " + oneLight_->text());
  change_->setText("");
  
}

//selects light 2 to change
void SingleSchedulerControlWidget::lightTwo() {
  Datalight = '2';
  light_->setText("You are changing Light 2     " + twoLight_->text());
  change_->setText("");
  
}

//selects light 3 to change
void SingleSchedulerControlWidget::lightThree() {
  Datalight = '3';
  light_->setText("You are changing Light 3     " + threeLight_->text());
  change_->setText("");
}

//turns light on
void SingleSchedulerControlWidget::on() {
  Dataon = 1; 
  change_->setText("Light: ON");
}

//turns light off
void SingleSchedulerControlWidget::off() {
  Dataon = 2;
  change_->setText("Light: OFF");
}

//changes the hue 
void SingleSchedulerControlWidget::hue() {
  int input = hueScaleSlider_->value();

  Datahue = input;
  change_->setText("new Hue: " + to_string(input));
}

//changes the brightness
void SingleSchedulerControlWidget::bright() {
  int input = briScaleSlider_->value();
  Databri = input;
  change_->setText("new Brightness: " + to_string(input));
}

//changes the saturation
void SingleSchedulerControlWidget::sat() {
  int input = satScaleSlider_->value();
  Datasat = input;
  change_->setText("new Saturation: " + to_string(input));
}

//changes the transition time
void SingleSchedulerControlWidget::transition() {
  int input = transitionScaleSlider_->value();
  Datatransition = input;
  change_->setText("new Transition Time: " + to_string(input * 100) + "ms");

}

void SingleSchedulerControlWidget::changeHour(){
  Datahour = hourInput_->currentText().toUTF8();
}

void SingleSchedulerControlWidget::changeMin(){
  Datamin = minInput_->currentText().toUTF8();
}

void SingleSchedulerControlWidget::changeSec(){
  Datasec = secInput_->currentText().toUTF8();
}

void SingleSchedulerControlWidget::createSchedule(){
  if ( Datalight == NULL) {
   light_->setText("Please select a light to change");
   change_->setText("");
  } else {
   Http::Client *client = SingleSchedulerControlWidget::connect();
   Http::Message *msg = new Http::Message();
  

   change_->setText(createPostMessage());
   msg->addBodyText(createPostMessage());
   client->done().connect(boost::bind(&SingleSchedulerControlWidget::handleHttpResponseVOID, this, _1, _2));
   if (scheduleID == "99"){
     client->post("http://" + ip + ":" + port + "/api/" + userID + "/schedules/", *msg);    
   }
   else{
      client->put("http://" + ip + ":" + port + "/api/" + userID + "/schedules/"+ scheduleID, *msg);    
   }
  }
}




std::string SingleSchedulerControlWidget::createPostMessage(){
  std::string postMessage = "{";
  if(scheduleID == "99"){
      Wt::log("info") << "Name Changed";
    postMessage += "\"name\": \"" +nameID +"\" ,";
  } 
  postMessage += "\"command\": {";
  postMessage += "\"address\": \"/api/" + userID + "/lights/" + Datalight + "/state\" ,";
  postMessage += "\"method\": \"PUT\","; 
  postMessage += "\"body\":" + createBodyMessage(); 
  postMessage += "},";
  postMessage += createDateTime(); 
  postMessage += "}"; 
  return postMessage; 
}

//Creates the light changes in a string to input into body of post
std::string SingleSchedulerControlWidget::createBodyMessage(){
  std::string bodyText = "{"; 

  if (Dataon != NULL){
    if (Dataon == 1){
       bodyText += "\"on\": true,";
    }
    else if (Dataon == 2){
       bodyText += "\"on\": false,";
    }
  }
  if (Datahue != NULL){
      bodyText += "\"hue\":"+ to_string(Datahue)+",";
  }
  if (Databri != NULL){
      bodyText += "\"bri\":"+ to_string(Databri)+",";
  }
  if (Datasat != NULL){
      bodyText += "\"sat\":"+ to_string(Datasat)+",";
  }
  if (Datatransition != NULL){
      bodyText += "\"transitiontime\":"+ to_string(Datatransition)+",";
  }
  bodyText.pop_back();
  bodyText += "}"; 
  return bodyText; 

}

std::string SingleSchedulerControlWidget::createDateTime(){
  std::string dateTimeMessage = "\"time\":"; 
  this->changeHour();
  this->changeMin(); 
  this->changeSec(); 

  /*int yy = Datayear;
  int m = Datamonth;
  int d = Dataday;
  m = (m + 9) % 12;
  yy = yy - m / 10;
  int daynum = 365 * yy + yy / 4 - yy / 100 + yy / 400 + (m * 306 + 5) / 10 + (d - 1) + count;

  int y = (10000 * daynum + 14780) / 3652425;
  int ddd = daynum - (365 * y + y / 4 - y / 100 + y / 400);
  if (ddd < 0) {
	  y = y - 1;
	  ddd = daynum - (365 * y + y / 4 - y / 100 + y / 400);
  }
  int mi = (100 * ddd + 52) / 3060;
  int mm = (mi + 2) % 12 + 1;
  y = y + (mi + 2) / 12;
  int dd = ddd - (mi * 306 + 5) / 10 + 1;

  dateTimeMessage += "\""+ to_string(y)+"-"+to_string(mm)+"-"+to_string(dd);*/
  dateTimeMessage += "\"" + to_string(Datayear) + "-" + to_string(Datamonth) + "-" + to_string(Dataday);
  dateTimeMessage += "T";
  if ((amSelector_->currentText().toUTF8()).compare("PM") == 0 ){
    int currentHour = stoi(Datahour);
    if (currentHour<12){
      currentHour += 12; 
      Datahour = to_string(currentHour); 
    }
    
  }
  dateTimeMessage += Datahour + ":"+Datamin + ":" + Datasec + "\"";
  return dateTimeMessage; 

}

// Function Name: deleteGroup()
// Parameters: none
// Return: none
// Description: Deletes a group
void SingleSchedulerControlWidget::deleteSchedule() {

  if (!deleteConfirm) {
    change_->setText("You are about to delete this schedule. Are you sure?");
    deleteConfirm = true;
  } else {
      Http::Client *client = SingleSchedulerControlWidget::connect();
       Http::Message *msg = new Http::Message();
     if(client->deleteRequest("http://" + ip + ":" + port + "/api/" + userID + "/schedules/"+ scheduleID, *msg)){
     }

     }
}


void SingleSchedulerControlWidget::returnBridge()
{
  clear();
  WApplication::instance()->setInternalPath("/light?user=" + userID + "%26ip=" + ip + "%26port=" + port, true);
}
