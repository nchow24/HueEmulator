// SchedulesControl.h : Defines the SchedulesControl class that manages the creation and listing of Schedules
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef SCHEDULERCONTROL_H_
#define SCHEDULERCONTROL_H_

class Session;

class SchedulerControlWidget: public Wt::WContainerWidget
{
public:

	/** @brief creates a SchedulerControlWidget
	*
	*  creates a SchedulerControlWidget that is tied to a database session. The Widget also has a parent Container Widget
	*
	*  @param session the database session
	*  @param parent the parent Widget Container
	*  @return SchedulerControlWidget
	*/
	SchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	/** @brief loads SingleSchedulesControlWidget page
	*
	*  loads the SchedulerControlWidget page which displays the list of current groups and the option to create a new group
	*
	*  @return Void
	*/
	void update();

private:
	int numOfSchedules;                                                /*!< keeps track of number of Schedules */
	Session *session_;                                                 /*!< keeps track of current session */
	std::string ip = "";											   /*!< keeps track of IP  */
	std::string userID = ""; 										   /*!< keeps track of UserID */                                      
	std::string port = "";											   /*!< keeps track of Port Number */
	bool one = false;											       /*!< keeps track of light 1 */
	bool two = false;											       /*!< keeps track of light 2 */
	bool three = false;											       /*!< keeps track of light 3 */
	Wt::WLineEdit *nameEdit_;										   /*!< displays name */
	Wt::WPushButton *createButton;								       /*!< displays a button to create Schedule*/
	Wt::WText *status_;											       /*!< displays current status */
	Wt::WText *Schedules_;											   /*!< displays the schedules */
	/** @brief creates an HTTP Client
	*
	*  creates an HTTP client that will be used to send either a post request to create a new group or a get request to retrieve the currently existing groups
	*
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();		                              

	/** @brief handles response and displays group information
	*
	*  gets the list of groups and displays each one as button that leads to the SingleSchedulerControlWidget where user can edit a specific group
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief handles response and displays group information
	*
	*  gets the list of groups and displays each one as button that leads to the SingleSchedulerWidget where user can edit a specific group
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	

	/** @brief creates a schedule
	*
	*  Creates a schedule based on information you enter
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void createSchedule();

	/** @Returns to bridge page
	*
	*  Returns to Bridge Page
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void returnBridge();
};

#endif
