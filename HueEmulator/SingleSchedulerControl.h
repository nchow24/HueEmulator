/** @file SchedulerControl.h
*  @brief Application for creating and listing schedules
*  @author Weija Zhou
*  @date Nov 28, 2017
*/

#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


#ifndef SINGLESCHEDULERCONTROL_H_
#define SINGLESCHEDULERCONTROL_H_


class Session;

class SingleSchedulerControlWidget: public Wt::WContainerWidget
{
public:
  /** @brief creates a SingleSchedulerControlWidget
  *
  *  creates a SingleSchedulerControlWidget that is tied to a database session. The Widget also has a parent Container Widget
  *
  *  @param session the database session
  *  @param parent the parent Widget Container
  *  @return SingleSchedulerControlWidget
  */
  SingleSchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
  
  /** @brief loads SingleSchedulerControlWidget page
  *
  *  loads the SingleSchedulerControlWidget page which displays changes to a light's name, and options to change a light's state
  *
  *  @return Void
  */
  void update();

private:
	Session *session_;										/*!< keeps track of light status */
	Wt::WLineEdit *nameEdit_;								/*!< light's name to be changed */
	Wt::WLineEdit *hueEdit_;								/*!< Light's hue selection */
	Wt::WSlider *satScaleSlider_;							/*!< Light's sat selection */
	Wt::WSlider *briScaleSlider_;							/*!< Light's brightness selection */
	Wt::WSlider *hueScaleSlider_;							/*!< Light's hue selection */
	Wt::WSlider *transitionScaleSlider_;					/*!< Light's transition selection */
	Wt::WCalendar *calendar_;								/*!< Calendar for selecting date */
	Wt::WComboBox *hourInput_ ;								/*!< Select Hour */
	Wt::WComboBox *minInput_ ;								/*!< Select Min */
	Wt::WComboBox *secInput_ ;								/*!< Select Sec */
	Wt::WComboBox *amSelector_; 							/*!< Select Am/Pm */
	Wt::WComboBox *reoccurChoices_;							/*!< Select number of reoccurences*/
	Wt::WText *oneLight_;									/*!< FirstLight */
	Wt::WText *twoLight_;									/*!< SecondLight */
	Wt::WText *threeLight_;									/*!< Third Light */
	Wt::WText *change_;										/*!< Displays Status of Light*/
	Wt::WText *light_;										/*!< Displays the selected Light */
	Wt::WText *dateSelect_; 								/*!< Displays Date */
	Wt::WText *scheduleInfoEdit_;							/*!< Displays the schedule Info */
	Wt::WText *scheduleTimeEdit_;							/*!< Displays the Schedule Time Info */
	Wt::WPushButton *deleteButton;							/*!< Delete Button */
	Wt::WPushButton *scheduleButton; 						/*!< Schedule Button */
	std::string currentLight = "0";							/*!< Variable for Light */
	std::string ip = "";									/*!< Variable for ip */
	std::string userID = "";								/*!< Variable for userID */
	std::string port = "";									/*!< Variable for Port */
	std::string scheduleID = "";							/*!< Variable for ScheduleID */
	std::string nameID = "";								/*!< Variable for NameID */
	std::string name; 										/*!< Variable for name of light */
  	char Datalight; 										/*!< Variable for schedule information */
  	int Dataon; 											/*!< Variable for schedule information */
  	int Datahue; 											/*!< Variable for schedule information */
  	int Databri;											/*!< Variable for schedule information */
  	int Datasat;											/*!< Variable for schedule information */
  	int Datatransition; 									/*!< Variable for schedule information */
  	int Datayear;											/*!< Variable for schedule information */
  	int Datamonth;											/*!< Variable for schedule information */
  	int Dataday; 											/*!< Variable for schedule information */
 	std::string Datahour; 									/*!< Variable for schedule information */
  	std::string Datamin; 									/*!< Variable for schedule information */
  	std::string Datasec; 									/*!< Variable for schedule information */
  	std::string DatamerDes; 								/*!< Variable for schedule information */
	bool deleteConfirm; 									/*!< Variable for confirming Delete */
	

	/** @brief Turns light on
	*
	*  Turns light on.
	*
	*  @return Void
	*/
	void on();


	/** @brief Turns light off
	*
	*  Turns light off.
	*
	*  @return Void
	*/
	void off();


	/** @brief Changes light Hue
	*
	*  Changes light Hue
	*
	*  @return Void
	*/
	void hue();

	/** @brief Changes light Bright
	*
	*  Changes light Bright
	*
	*  @return Void
	*/
	void bright();

	/** @brief Changes light Saturation
	*
	*  Changes light Saturation
	*
	*  @return Void
	*/
	void sat();

	/** @brief Changes light Transition
	*
	*  Changes light Transition
	*
	*  @return Void
	*/
	void transition();

	/** @brief Handles Https Reponse V1
	*
	*  Handles the Https Response for basic
	*
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	
	/** @brief Handles Https Reponse V2
	*
	*  Handles the Https Response for Name
	*
	*  @return Void
	*/
	void handleHttpResponseName(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief Handles Https Reponse V3
	*
	*  Handles the Https Response for Void
	*
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief Handles Https Connect 
	*
	*  Connects to Https
	*
	*  @return Void
	*/
	Wt::Http::Client * connect();

	/** @brief Changes Light One
	*
	*  Change Light One
	*
	*  @return Void
	*/
	void lightOne();

	/** @brief Changes Light Two
	*
	*  Change Light Two
	*
	*  @return Void
	*/
	void lightTwo();

	/** @brief Changes Light Three
	*
	*  Change Light Three
	*
	*  @return Void
	*/
	void lightThree();

	/** @brief Returns back to Bridge Page
	*
	*  This function returns user to Bridge Page
	*
	*  @return Void
	*/
	void returnBridge();

	/** @brief Changes the current Date
	*
	*  This function changes the Date Variable
	*
	*  @return Void
	*/
	void changeDate(); 

	/** @brief Creates a new schedules
	*
	*  Will create a schedule that is stored into the emulator
	*
	*  @return Void
	*/
	void createSchedule(); 

	/** @brief Changes the hour
	*
	*  Changes the current Hour. 
	*
	*  @return Void
	*/
	void changeHour(); 

	/** @brief Changes the Minute value
	*
	*  Changes the current Minutes. 
	*
	*  @return Void
	*/
	void changeMin(); 

	/** @brief Changes the Seconds value
	*
	*  Changes the current Seconds. 
	*
	*  @return Void
	*/
	void changeSec(); 

	/** @brief Deletes the current Schedule
	*
	*  Removes the schedule from the emulator
	*
	*  @return Void
	*/
	void deleteSchedule();

	/** @brief Creates a get Signal
	*
	*  Creates the body of the get signal
	*
	*  @return Void
	*/
	std::string createBodyMessage();

	/** @brief Creates a Post Signal
	*
	*  Creates the body of the post signal
	*
	*  @return Void
	*/
	std::string createPostMessage();

	/** @brief Creates a Date and Time 
	*
	*  Creates the body of the post signal
	*
	*  @return Void
	*/
	std::string createDateTime();
	int count;
	int choice;


};

#endif
