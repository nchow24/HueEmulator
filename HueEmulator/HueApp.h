#include <Wt/WContainerWidget>

#include "Session.h"
#include "LightsControl.h"
#include "BridgeControl.h"
#include "GroupsControl.h"
#include "SingleGroupsControl.h"
#include "BridgeEditControl.h"
#include "SchedulerControl.h"
#include "SingleSchedulerControl.h"
#include "GroupsSchedulerControl.h"



#ifndef HUEAPP_H_
#define HUEAPP_H_




namespace Wt {
  class WStackedWidget;
  class WAnchor;
}

class Session;
class LightsControlWidget; 

/**
* @class BridgeControlWidget
* @brief Function prototypes for the BridgeControl class that allows bridges to be selected and new ones to be registered
*
* Contains functions and members to display the names of current bridges as buttons and a form to create new bridges. These buttons each lead to pages manage different components of the bridge.
* This can be groups, schedules, lights, and editing the bridge. The form will check for valid input and when valid information is given, will register the bridge and add its information to the database
**/
class BridgeControlWidget;
class GroupsControlWidget;
class SingleGroupsControlWidget;
/**
*  @class BridgeEditControlWidget
*  @brief Function prototypes for the BridgetEditControl class that manages the updating of a bridge
*
*  Contains functions and members to manipulate a bridge's information which is stored in a database. This is done by providing a form filled in with the bridge's current information
*  so any changes the user makes can be saved into the database. A button is provided to apply these changes and another returns back to the page where bridge selection and registration is done
*  @author Daniel Le
*  @date Nov 28, 2017
*/
class BridgeEditControlWidget;
class SchedulerControlWidget;
class SingleSchedulerControllWidget;
class GroupsSchedulerControlWidget;


class HueApp : public Wt::WContainerWidget
{
public:
  HueApp(Wt::WContainerWidget *parent = 0);

  void handleInternalPath(const std::string &internalPath);

private:
  Wt::WStackedWidget *mainStack_;
  LightsControlWidget *the_Lights;
  BridgeControlWidget *the_Bridge;						/*!< The parent widget for the bridge registering and selection page*/
  GroupsControlWidget *the_Groups;
  SingleGroupsControlWidget *the_SingleGroups;
  BridgeEditControlWidget *the_BridgeEdit;				/*!< The parent widget for the bridge editing page*/
  SchedulerControlWidget * the_Schedulers; 
  SingleSchedulerControlWidget * the_SingleSchedulers; 
  GroupsSchedulerControlWidget * the_GroupSchedulers; 

  Wt::WAnchor *backToGameAnchor_;

  Session session_;

  void onAuthEvent();
  void showLights();

  /**
  * @brief Displays the bridge registration page
  *
  * Loads in the widgets for the page to register the bridge. There is a form to enter a name, IP address, port number, and location as well as a register button.
  * Above the form is a list of all bridges that are already in the database. Clicking on one of these items redirects to a page that allows the user to modify the lights on that bridge
  * @return Void.
  **/
  void showBridge();
  void showGroups();
  void showSingleGroups();

  /**
  * @brief Displays the bridge editing page
  *
  * Loads in the widgets for the page to edit the selected bridge. There is a form to enter a name, IP address, port number, and location that is already filled with the corresponding information
  * for the bridge. There is a button to save the changes made to this bridge, updating its info in the database base as well as a button that returns back to the homepage
  * @return Void.
  **/
  void showBridgeEdit();
  void showSchedulers(); 
  void showSingleSchedulers(); 
  void showGroupScheduler(); 

};

#endif //HUEAPP_H_
