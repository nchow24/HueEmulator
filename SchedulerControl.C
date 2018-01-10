/** @file SchedulerControl.C
*  @brief Application for creating and listing schedules
*  @author Weija Zhou
*  @date Nov 28, 2017
*/


#include <boost/lexical_cast.hpp>
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
#include <Wt/Http/Client>
#include "SchedulerControl.h"
#include "Session.h"
#include <algorithm>


using namespace Wt;
using namespace std;

SchedulerControlWidget::SchedulerControlWidget(Session *session, WContainerWidget *parent) :
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
void SchedulerControlWidget::update()
{
  clear();

  //get URL info
  string address = WApplication::instance()->internalPath();
  size_t pos = address.find("user=");               //get userID
  string subString = address.substr(pos + 5);
  size_t endPos = subString.find("&");
  userID = subString.substr(0, endPos);
  pos = address.find("ip=");                    //get ip
  subString = address.substr(pos + 3);
  endPos = subString.find("&");
  ip = subString.substr(0, endPos);
  pos = address.find("port=");                  //get port
  subString = address.substr(pos + 5);
  endPos = subString.find("&");
  port = subString.substr(0, endPos);

  one = false;
  two = false;
  three = false; 

  //display user info in top left corner
  string firstName = session_->firstName();
  string lastName = session_->lastName();
  WText *userInfo_ = new WText(this);
  userInfo_->setTextAlignment(AlignmentFlag::AlignLeft);
  userInfo_->setText("Hello, " + firstName + " " + lastName);
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //return to lights page
  WPushButton *lightButton
    = new WPushButton("Return to My Lights", this);
  lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
  lightButton->setMargin(10, Left);
  
  //return to bridge page
  WPushButton *returnButton           
    = new WPushButton("Return To Bridge", this);
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //create a new Schedule
  this->addWidget(new WText("CREATE A SCHEDULE: "));
  this->addWidget(new WBreak());
  this->addWidget(new WText("Schedule name: "));
  nameEdit_ = new WLineEdit(this);                        
  nameEdit_->setFocus();
  this->addWidget(new WBreak());

  //select the lights to be part of the Schedule
  // this->addWidget(new WText("Choose your lights: "));
  // WPushButton *oneButton
  //   = new WPushButton("Light 1", this);                   // 1st light button
  // oneButton->setMargin(5, Left);
  // WPushButton *twoButton
  //   = new WPushButton("Light 2", this);                   // 2nd light button
  // twoButton->setMargin(5, Left);
  // WPushButton *threeButton
  //   = new WPushButton("Light 3", this);                   // 3rd light button
  // threeButton->setMargin(5, Left);
  // this->addWidget(new WBreak());
  
  // // displays which lights have been chosen
  // this->addWidget(new WText("Selected lights for your new Schedule: "));
  // this->addWidget(new WBreak());
  // light1_ = new WText(this);                          
  // this->addWidget(new WBreak());
  // light2_ = new WText(this);
  // this->addWidget(new WBreak());
  // light3_ = new WText(this);
  

  //create Schedule
  this->addWidget(new WBreak());
  createButton = new WPushButton("Create Schedule", this);                  
  this->addWidget(new WBreak());

  status_ = new WText(this);                           //status of creating a Schedule
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());

  //list Schedules
  this->addWidget(new WText("Your Schedules(Click to Edit): "));
  this->addWidget(new WBreak());
  this->addWidget(new WBreak());
  Schedules_ = new WText(this);
  Http::Client *client = SchedulerControlWidget::connect();
  client->done().connect(boost::bind(&SchedulerControlWidget::handleHttpResponse, this, _1, _2));
  if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/schedules")) {
    WApplication::instance()->deferRendering();
  }


  createButton->clicked().connect(this, &SchedulerControlWidget::createSchedule);

  returnButton->clicked().connect(this, &SchedulerControlWidget::returnBridge);

  (boost::bind(&SchedulerControlWidget::handleHttpResponse, this));

  (boost::bind(&SchedulerControlWidget::handleHttpResponseVOID, this));

  (boost::bind(&SchedulerControlWidget::connect, this));

  (boost::bind(&SchedulerControlWidget::createSchedule, this));

  (boost::bind(&SchedulerControlWidget::returnBridge, this));

}

// Function Name: connect() 
// Parameters: none
// Return: none
// Description: creates an Http client
Http::Client * SchedulerControlWidget::connect() {

  Http::Client *client = new Http::Client(this);
  client->setTimeout(15);
  client->setMaximumResponseSize(10 * 1024);
}

// Function Name: handleHttpResponseVOID()
// Parameters: none
// Return: none
// Description: reloads the same Widget with changes to the Schedule list
void SchedulerControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {

  update();
}

// Function Name: handleHttpResponse()
// Parameters: none
// Return: none
// Description: displays the list of Schedules as buttons 
void SchedulerControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {

  WApplication::instance()->resumeRendering();
  if (!err && response.status() == 200) {

    //find number of Schedules
    string jsonMessage = response.body(); 
    int count =0; 
    for (int i = 0; i < jsonMessage.size(); i++){
      if (jsonMessage[i] == '{'){
        count ++; 
      }
    }
    int n= count-1; 
    numOfSchedules = n; 

    // status_->setText(num); 
    //create a button for each Schedule that leads to the ability to edit that specific Schedule
    for (int i = 0; i < n; i++) {
      string Schedules = response.body();
      if (Schedules.find("\"" + to_string(i + 1) + "\":") != string::npos) {
        size_t pos = Schedules.find("\"" + to_string(i + 1) + "\"");
        string subString;
        if ( (i + 1) >= 10) {
          subString = Schedules.substr(pos + 14);
        } else {
          subString = Schedules.substr(pos + 13);
        }
        size_t endPos = subString.find("\"");
        string name = subString.substr(0, endPos);
        WPushButton *currentButton = new WPushButton(to_string(i +1)+"-"+name, this);
        currentButton->setMargin(5, Left);
        currentButton->setLink("/?_=/singlescheduler?user=" + userID + "%26ip=" + ip + "%26port=" + port + "%26scheduleid=" + to_string(i+1)+ "%26name="+ name);
      }
    }

  }
  
}

// Function Name: createSchedule()
// Parameters: none
// Return: none
// Description: creates a new Schedule
void SchedulerControlWidget::createSchedule() {

  //determine which lights have been chosen
    if (nameEdit_->text().toUTF8() == "") {
      status_->setText("Enter a name for your Schedule");
    } else {
      status_->setText("Are you sure?");
      // WApplication::instance()->setInternalPath("/singlescheduler?user=" + userID + "%26ip=" + ip + "%26port=" + port + "%26scheduleid=" + to_string(99) + "%26name="+ nameEdit_->text().toUTF8(), true);
      createButton->setLink("/?_=/singlescheduler?user=" + userID + "%26ip=" + ip + "%26port=" + port + "%26scheduleid=" + to_string(99) + "%26name="+ nameEdit_->text().toUTF8());

    }
}



// Function Name: returnBridge()
// Parameters: none
// Return: none
// Description: goes back to bridge page
void SchedulerControlWidget::returnBridge() {

  clear();
  WApplication::instance()->setInternalPath("/Bridge", true);
}
