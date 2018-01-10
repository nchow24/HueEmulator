/** @file groupsScheduleControl.h
*  @brief Application for creating and listing schedules of Groups
*  @author Weija Zhou
*  @date Nov 28, 2017
*/

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef GROUPSSCHEDULERCONTROL_H_
#define GROUPSSCHEDULERCONTROL_H_

class Session;

class GroupsSchedulerControlWidget: public Wt::WContainerWidget
{
public:
  /** @brief creates a GroupsSchedulerControlWidget
  *
  *  creates a GroupsSchedulerControlWidget that is tied to a database session. The Widget also has a parent Container Widget
  *
  *  @param session the database session
  *  @param parent the parent Widget Container
  *  @return GroupsSchedulerControlWidget
  */
  GroupsSchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
  
  /** @brief loads GroupsSchedulerControlWidget page
  *
  *  loads the GroupsSchedulerControlWidget page which displays changes to a light's name, and options to change a light's state
  *
  *  @return Void
  */
  void update();

private:
  Session *session_;                      /*!< keeps track of light status */
  std::string ip = "";                    /*!< Variable for ip */
  std::string userID = "";                /*!< Variable for userID */
  std::string port = "";                  /*!< Variable for Port */
  std::string groupID = "";               /*!< Variable for groupID */
  std::string lights;                     /*!< Variable for lights */
  bool deleteConfirm;                     /*!< Delete Confirmation */
  Wt::WText *groupInfoEdit_;              /*!< Displays the group Info */
  Wt::WText *groupLightsEdit_;            /*!< Displays the group Lights */
  Wt::WCalendar *calendar_;               /*!< Displays the Calendar */
  Wt::WText *dateSelect_;                 /*!< Displays the DateSelect */
  Wt::WComboBox *hourInput_ ;             /*!< Displays the Hour Input */
  Wt::WComboBox *minInput_ ;              /*!< Displays the Min Input */
  Wt::WComboBox *secInput_ ;              /*!< Displays the Seconds Input */
  Wt::WComboBox *amSelector_;             /*!< Displays the Am/Pm Selector */
  Wt::WLineEdit *hueEdit_;                /*!< Displays the Hue Edit */
  Wt::WSlider *satScaleSlider_;           /*!< Creates Slider for Saturations */
  Wt::WSlider *briScaleSlider_;           /*!< Creates Slider for Brightness */
  Wt::WSlider *hueScaleSlider_;           /*!< Creates Slider for Hue */
  Wt::WSlider *transitionScaleSlider_;    /*!< Creates Slider for Transition Time */
  Wt::WText *change_;                     /*!< Displays the Current Change */
  Wt::WPushButton *scheduleButton;        /*!< Create Schedule Button */


  int stateOn;                            /*!< Variable for Light State */
  int stateHue;                           /*!< Variable for Hue */
  int stateBri;                           /*!< Variable for Brightness */
  int stateSat;                           /*!< Variable for Saturation */
  int stateTrans;                         /*!< Variable for Transition */
  int Datayear;                           /*!< Variable for Year */
  int Datamonth;                          /*!< Variable for Month */
  int Dataday;                            /*!< Variable for Day */
  std::string Datahour;                   /*!< Variable for Hour of Data */
  std::string Datamin;                    /*!< Variable for Min of Data */
  std::string Datasec;                    /*!< Variable for Sec of Data */
  std::string DatamerDes;                 /*!< Variable for AM/PM of Data */

  /** @brief Handles Https Connect 
  *
  *  Connects to Https
  *
  *  @return Void
  */
  Wt::Http::Client * connect();
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
  void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);

  /** @brief Handles Https Reponse V3
  *
  *  Handles the Https Response for Void
  *
  *  @return Void
  */
  void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);

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

  /** @brief Changes Music
  *
  *  Music that plays
  *
  *  @return Void
  */  
  static void music(Wt::WSound *sound);


  /** @brief Adds Light to Group
  *
  *  Will add light to Group
  *
  *  @return Void
  */  
  void addLights();

  /** @brief Removes the Lights
  *
  *  Removes Light from group
  *
  *  @return Void
  */  
  void removeLights();

  /** @brief Delete Group
  *
  *  Will Delete Group
  *
  *  @return Void
  */  
  void deleteGroup();

  /** @brief Returns to Bridge Page
  *
  *  Will Return to Bridge Page
  *
  *  @return Void
  */  
  void returnBridge();

  /** @brief Changes Transition
  *
  * Changes Variable of Transition
  *
  *  @return Void
  */  
  void transition();

  /** @brief Changes Date
  *
  * Will change the date of the schedule
  *
  *  @return Void
  */  
  void changeDate(); 

  /** @brief Creates a new schedule
  *
  *  New Schedule is Created
  *
  *  @return Void
  */  
  void createSchedule(); 

  /** @brief Changes Hour
  *
  *  Changes the Current Hour
  *
  *  @return Void
  */  
  void changeHour(); 

  /** @brief Changes Minut
  *
  *  Changes the current Minute
  *
  *  @return Void
  */  
  void changeMin(); 

  /** @brief Changes Seconds
  *
  *  Changes the Current Second
  *
  *  @return Void
  */  
  void changeSec(); 

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

};

#endif
