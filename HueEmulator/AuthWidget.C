/** @file AuthWidget.C
*  @brief Creates a registration view for the user to interact with
*
* Create a custom AuthWidget to override the pre-existing Auth::authWidget.
* This allows us to add a custom registration page to take in more data for 
* the web app.
*
*  @author Paul Li
*  @date Nov 28, 2017
*/
#include <Wt/Auth/RegistrationModel>
#include <Wt/WPushButton>
#include "AuthWidget.h"
#include "RegistrationView.h"
#include "Session.h"

/** @brief Constructor for AuthWidget
 *
 *  Calls the constructor while assigning the AuthWidget and Session
 *  
 *  @param session is the current session data of the app.
 */
AuthWidget::AuthWidget(Session& session)
  : Wt::Auth::AuthWidget(Session::auth(), session.users(), session.login()),
    session_(session)
{  }
/** @brief Creates all of the registration view.
 *
 *  Overrides the existing Auth::createRegistrationView to call our custom registrationView class.
 *  
 *  @param id takes a user identity.
 */
Wt::WWidget *AuthWidget::createRegistrationView(const Wt::Auth::Identity& id)
{
  RegistrationView *w = new RegistrationView(session_, this);
  Wt::Auth::RegistrationModel *model = createRegistrationModel();

  if (id.isValid()){
    model->registerIdentified(id);
    model->setEmailPolicy(Wt::Auth::RegistrationModel::EmailPolicy::EmailMandatory);

  }
	

  w->setModel(model);
  return w;
}