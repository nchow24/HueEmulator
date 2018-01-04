/** @file groupsScheduleControl.C
*  @brief Application for creating and listing schedules of Groups
*  @author Weija Zhou
*  @date Nov 28, 2017
*/
#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WSound>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>
#include <Wt/WSlider>
#include <Wt/WCalendar>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Http/Client>
#include "GroupsSchedulerControl.h"
#include "Session.h"

using namespace Wt;
using namespace std;

GroupsSchedulerControlWidget::GroupsSchedulerControlWidget(Session *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session)
{
	setContentAlignment(AlignCenter);
	setStyleClass("highscores");
}

// Function Name: update()
// Parameters: none
// Return: none
// Description: generates the Widget
void GroupsSchedulerControlWidget::update()
{
	clear();

	//get user info from URL
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("user=");								//get userID
	string subString = address.substr(pos + 5);
	size_t endPos = subString.find("&");
	userID = subString.substr(0, endPos);
	pos = address.find("ip=");										//get ip
	subString = address.substr(pos + 3);
	endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");									//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);
	pos = address.find("groupid=");									//get groupID
	subString = address.substr(pos + 8);
	endPos = subString.find("&");
	groupID = subString.substr(0, endPos);

	deleteConfirm = false;

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
	Http::Client *client = GroupsSchedulerControlWidget::connect();
	client->done().connect(boost::bind(&GroupsSchedulerControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID)) {
		WApplication::instance()->deferRendering();
	}
	groupInfoEdit_ = new WText(this);								//group name
	this->addWidget(new WBreak());
	groupLightsEdit_ = new WText(this);								//lights in the group
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
	hueScaleSlider_ = new WSlider(this);					 //slider bar
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
	briScaleSlider_ = new WSlider(this);					
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
	satScaleSlider_ = new WSlider(this);				
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
	transitionScaleSlider_ = new WSlider(this);					
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
	this->addWidget(new WBreak());
	
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
	
	 this->addWidget(new WBreak());
  	dateSelect_ = new WText(this);
  	dateSelect_->setText("Selected Date:          ");
  	this->addWidget(new WBreak());
  	calendar_ = new WCalendar(this);
  	calendar_->setSingleClickSelect(true);
  	this->addWidget(new WBreak());

	

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	change_ = new WText(this);                          //status of a light change
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	 scheduleButton = new WPushButton("Create Schedule", this);                      
  	onButton->setMargin(5, Left);
	this->addWidget(new WBreak());

	//return to groups page
	WPushButton *groupButton
		= new WPushButton("Return to My Groups", this);
	groupButton->setLink("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	groupButton->setMargin(10, Left);
	
	//return to lights page
	WPushButton *lightButton
		= new WPushButton("Return to My Lights", this);
	lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	lightButton->setMargin(10, Left);
	WPushButton *returnButton							
		= new WPushButton("Return To Bridge", this);

	onButton->clicked().connect(this, &GroupsSchedulerControlWidget::on);
	
	offButton->clicked().connect(this, &GroupsSchedulerControlWidget::off);
	returnButton->clicked().connect(this, &GroupsSchedulerControlWidget::returnBridge);
	briScaleSlider_->valueChanged().connect(this, &GroupsSchedulerControlWidget::bright);
	satScaleSlider_->valueChanged().connect(this, &GroupsSchedulerControlWidget::sat);
	hueScaleSlider_->valueChanged().connect(this, &GroupsSchedulerControlWidget::hue);
	transitionScaleSlider_->valueChanged().connect(this, &GroupsSchedulerControlWidget::transition);
	calendar_->clicked().connect(this, &GroupsSchedulerControlWidget::changeDate);
  	hourInput_->changed().connect(this, &GroupsSchedulerControlWidget::changeHour);
  	minInput_->changed().connect(this, &GroupsSchedulerControlWidget::changeMin);
  	secInput_->changed().connect(this, &GroupsSchedulerControlWidget::changeSec);
  	 scheduleButton->clicked().connect(this, &GroupsSchedulerControlWidget::createSchedule);

	(boost::bind(&GroupsSchedulerControlWidget::hue, this));
	(boost::bind(&GroupsSchedulerControlWidget::bright, this));
	(boost::bind(&GroupsSchedulerControlWidget::sat, this));
	(boost::bind(&GroupsSchedulerControlWidget::on, this));
	(boost::bind(&GroupsSchedulerControlWidget::off, this));
	(boost::bind(&GroupsSchedulerControlWidget::returnBridge, this));
	(boost::bind(&GroupsSchedulerControlWidget::handleHttpResponse, this));
	(boost::bind(&GroupsSchedulerControlWidget::handleHttpResponseUpdate, this));
	(boost::bind(&GroupsSchedulerControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&GroupsSchedulerControlWidget::connect, this));
	(boost::bind(&GroupsSchedulerControlWidget::transition, this));
	(boost::bind(&GroupsSchedulerControlWidget::changeDate, this));
  	(boost::bind(&GroupsSchedulerControlWidget::changeHour, this));
  	(boost::bind(&GroupsSchedulerControlWidget::changeMin, this));
  	(boost::bind(&GroupsSchedulerControlWidget::changeSec, this));
  	(boost::bind(&GroupsSchedulerControlWidget::createSchedule, this));
}

// Function Name: connect() 
// Parameters: none
// Return: none
// Description: creates an Http client
Http::Client * GroupsSchedulerControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

// Function Name: handleHttpResponseUpdate()
// Parameters: none
// Return: none
// Description: reloads the same Widget with changes to the group (for name/light member changes)
void GroupsSchedulerControlWidget::handleHttpResponseUpdate(boost::system::error_code err, const Http::Message& response) {
	update();
}

// Function Name: handleHttpResponseVOID()
// Parameters: none
// Return: none
// Description: empty function for responses that don't need to be processed (for on/off/hue/sat/bri/transition changes)
void GroupsSchedulerControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

// Function Name: handleHttpResponse()
// Parameters: none
// Return: none
// Description: parses and displays group information when update() is called
void GroupsSchedulerControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//get group name and lights in the group
		Json::Object result;
		Json::parse(response.body(), result);
		string name = result.get("name");
		size_t pos = response.body().find("lights");
		string subString = response.body().substr(pos + 10);
		size_t endPos = subString.find("]");
		lights = subString.substr(0, endPos);
		boost::erase_all(lights, "\"");
		groupInfoEdit_->setText("Group Name: " + name);
		groupLightsEdit_->setText("Lights in your Group: " + lights);
		change_->setText("");
	}
}


// Function Name: on()
// Parameters: none
// Return: none
// Description: turns lights on
void GroupsSchedulerControlWidget::on() {
	stateOn = 1; 
	change_->setText("Light: ON");
}

// Function Name: off()
// Parameters: none
// Return: none
// Description: turns lights off
void GroupsSchedulerControlWidget::off() {
	stateOn = 2; 
	change_->setText("Light: OFF");
}

// Function Name: hue()
// Parameters: none
// Return: none
// Description: changes group's hue
void GroupsSchedulerControlWidget::hue() {
	stateHue = hueScaleSlider_->value();
	change_->setText("new Hue: " + to_string(stateHue));
}

// Function Name: bright()
// Parameters: none
// Return: none
// Description: changes group's brightness
void GroupsSchedulerControlWidget::bright() {
	stateBri = briScaleSlider_->value();
	change_->setText("new Hue: " + to_string(stateBri));
}

// Function Name: sat()
// Parameters: none
// Return: none
// Description: changes group's saturation
void GroupsSchedulerControlWidget::sat(){
	stateSat = satScaleSlider_->value();
	change_->setText("new Hue: " + to_string(stateSat));
}

// Function Name: transition()
// Parameters: none
// Return: none
// Description: changes group's transition time
void GroupsSchedulerControlWidget::transition() {
	stateTrans = transitionScaleSlider_->value();
	change_->setText("new Hue: " + to_string(stateTrans));
}

void GroupsSchedulerControlWidget::createSchedule(){
  
   Http::Client *client = GroupsSchedulerControlWidget::connect();
   Http::Message *msg = new Http::Message();
   msg->addBodyText(createPostMessage());
   client->done().connect(boost::bind(&GroupsSchedulerControlWidget::handleHttpResponseVOID, this, _1, _2));
   client->post("http://" + ip + ":" + port + "/api/" + userID + "/schedules/", *msg);    
}




std::string GroupsSchedulerControlWidget::createPostMessage(){
  std::string postMessage = "{";
  postMessage += "\"name\": \"group\" ,";
  postMessage += "\"command\": {";
  postMessage += "\"address\": \"/api/" + userID + "/groups/" + groupID + "/action\" ,";
  postMessage += "\"method\": \"PUT\","; 
  postMessage += "\"body\":" + createBodyMessage(); 
  postMessage += "},";
  postMessage += createDateTime(); 
  postMessage += "}"; 
  return postMessage; 
}

//Creates the light changes in a string to input into body of post
std::string GroupsSchedulerControlWidget::createBodyMessage(){
  std::string bodyText = "{"; 

  if (stateOn != NULL){
    if (stateOn == 1){
       bodyText += "\"on\": true,";
    }
    else if (stateOn == 2){
       bodyText += "\"on\": false,";
    }
  }
  if (stateHue != NULL){
      bodyText += "\"hue\":"+ to_string(stateHue)+",";
  }
  if (stateBri != NULL){
      bodyText += "\"bri\":"+ to_string(stateBri)+",";
  }
  if (stateSat != NULL){
      bodyText += "\"sat\":"+ to_string(stateSat)+",";
  }
  if (stateTrans != NULL){
      bodyText += "\"transitiontime\":"+ to_string(stateTrans)+",";
  }
  bodyText.pop_back();
  bodyText += "}"; 
  return bodyText; 

}
void GroupsSchedulerControlWidget::changeHour(){
  Datahour = hourInput_->currentText().toUTF8();
}

void GroupsSchedulerControlWidget::changeMin(){
  Datamin = minInput_->currentText().toUTF8();
}

void GroupsSchedulerControlWidget::changeSec(){
  Datasec = secInput_->currentText().toUTF8();
}
void GroupsSchedulerControlWidget::changeDate() {
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

std::string GroupsSchedulerControlWidget::createDateTime(){
  std::string dateTimeMessage = "\"time\":"; 
  this->changeHour();
  this->changeMin(); 
  this->changeSec(); 
  dateTimeMessage += "\""+ to_string(Datayear)+"-"+to_string(Datamonth)+"-"+to_string(Dataday);
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
// Function Name: returnBridge()
// Parameters: none
// Return: none
// Description: goes back to bridge page
void GroupsSchedulerControlWidget::returnBridge(){
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
