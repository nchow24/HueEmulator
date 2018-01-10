/** @file HueApp.C
*  @brief the main application
*
*   Handles the main web application when it comes down to handling the routing, 
*   main view, authorization, login and loading other content of our web application.
*
*  @author Paul Li
*  @author Weija Zhou
*  @author Daniel Le
*  @author Nicole Chow
*  @date Nov 28, 2017
*/

#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WApplication>
#include <Wt/Auth/AuthWidget>

#include "HueApp.h"
#include "AuthWidget.h"

using namespace Wt;

/** @brief Constructor for HueApp.
 *
 *  The initialization of variables throughout our application. Also adding
 *  the overall layout for all the pages (the title). 
 *  
 *  @param parent takes in the parrent widget for the Hue App.
 */
HueApp::HueApp(WContainerWidget *parent):
  WContainerWidget(parent),
  the_Lights(0),
  the_Bridge(0),
  the_Groups(0),
  the_SingleGroups(0),
  the_BridgeEdit(0),
  the_Schedulers(0),
  the_SingleSchedulers(0),
  the_GroupSchedulers(0)

{
  session_.login().changed().connect(this, &HueApp::onAuthEvent);

  Auth::AuthModel *authModel = new Auth::AuthModel(Session::auth(),
               session_.users(), this);
  authModel->addPasswordAuth(&Session::passwordAuth());
  authModel->addOAuth(Session::oAuth());

  AuthWidget *authWidget = new AuthWidget(session_);
  authWidget->setModel(authModel);
  authWidget->setRegistrationEnabled(true);

  WText *title = new WText("<h1>NWPD Light App</h1>");
  addWidget(title);

  addWidget(authWidget);

  mainStack_ = new WStackedWidget();
  mainStack_->setStyleClass("gamestack");
  addWidget(mainStack_);

  WApplication::instance()->internalPathChanged()
    .connect(this, &HueApp::handleInternalPath);

  authWidget->processEnvironment();
}

/** @brief Checks if the user is logged in and will redirect you accordingly
 *
 *  Checks the session to see if anyone is logged in. If not it will clear all the widgets that
 *  might have been made before and bring you back to the login page. 
 */
void HueApp::onAuthEvent()
{
  if (session_.login().loggedIn()) {  
    handleInternalPath(WApplication::instance()->internalPath());
  } else {
    mainStack_->clear();
    the_Lights = 0;
    the_Bridge = 0;
    the_Groups = 0;
    the_SingleGroups = 0;
    the_BridgeEdit = 0;
    the_Schedulers = 0;
    the_SingleSchedulers = 0;
    the_GroupSchedulers = 0; 

  }
}

/** @brief Checks the url to redirect you to the correct page.
 *
 *  Checks the path in the url and wil create and load the widget accordingly if you are logged in.
 *  
 *  @param internalPath the url path.
 */
void HueApp::handleInternalPath(const std::string &internalPath)
{
  if (session_.login().loggedIn()) {
	  if (internalPath.find("/light") == 0)
		  showLights();
	  else if (internalPath.find("/bridge") == 0)
		  showBridge();
    else if (internalPath.find("/groupscheduler") == 0)
         showGroupScheduler();
	else if (internalPath.find("/group") == 0)
		  showGroups();
	else if (internalPath.find("/singlegroup") == 0)
		  showSingleGroups();
	else if (internalPath.find("/editbridge") == 0)
		  showBridgeEdit();
  	else if (internalPath.find("/scheduler") == 0)
    		 showSchedulers();
    else if (internalPath.find("/singlescheduler") == 0)
         showSingleSchedulers();
    else
      WApplication::instance()->setInternalPath("/bridge",  true);
  }
}

/** @brief Load Lights page.
 *
 *  Will redirect the user and create the widget for the lights page to display the content.
 */
void HueApp::showLights()
{
  if (!the_Lights)
    the_Lights = new LightsControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Lights);
  the_Lights->update();
}

/** @brief Load Brige page.
 *
 *  Will redirect the user and create the widget for the bridge page to display the content.
 */
void HueApp::showBridge(){
  if (!the_Bridge)
    the_Bridge = new BridgeControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Bridge);
  the_Bridge->update();
}

/** @brief Load groups page.
 *
 *  Will redirect the user and create the widget for the groups page to display the content.
 */
void HueApp::showGroups()
{
	if (!the_Groups)
		the_Groups = new GroupsControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_Groups);
	the_Groups->update();
}

/** @brief Load Single Groups page.
 *
 *  Will redirect the user and create the widget for the Single Groups page to display the content.
 */
void HueApp::showSingleGroups()
{
	if (!the_SingleGroups)
		the_SingleGroups = new SingleGroupsControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_SingleGroups);
	the_SingleGroups->update();
}

/** @brief Load edit bridge page.
 *
 *  Will redirect the user and create the widget for the edit bridge page to display the content.
 */
void HueApp::showBridgeEdit()
{
	if (!the_BridgeEdit)
		the_BridgeEdit = new BridgeEditControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_BridgeEdit);
	the_BridgeEdit->update();
}
/** @brief Load scheduler page.
 *
 *  Will redirect the user and create the widget for the scheduler page to display the content.
 */
void HueApp::showSchedulers(){
  if (!the_Schedulers)
    the_Schedulers = new SchedulerControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Schedulers);
  the_Schedulers->update();
}
/** @brief Load single scheduler page.
 *
 *  Will redirect the user and create the widget for the single scheduler page to display the content.
 */
void HueApp::showSingleSchedulers(){
  if (!the_Schedulers)
    the_SingleSchedulers = new SingleSchedulerControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_SingleSchedulers);
  the_SingleSchedulers->update();

}
/** @brief Load group scheduler page.
 *
 *  Will redirect the user and create the widget for the group scheduler page to display the content.
 */
void HueApp::showGroupScheduler(){
  if (!the_Schedulers)
    the_GroupSchedulers = new GroupsSchedulerControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_GroupSchedulers);
  the_GroupSchedulers->update();

}