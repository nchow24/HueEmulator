/** @file Main.C
*  @brief The main to run the server
*
*   It includes the bare essentials to start our server and the resources
*   that are used in our application.
* 
*  @author Nicole Chow
*  @author Paul Li
*  @author Weija Zhou
*  @author Daniel Le
*  @date Nov 28, 2017
*/

#include <Wt/WApplication>
#include <Wt/WServer>
#include <Wt/WAnchor>

#include "HueApp.h"
#include "Session.h"

/** @brief Create the application that is based off wt.
 *
 *  Creates the application and addes the resources that will be used in the application
 *  
 *  @param env the Wt enviornment.
 */
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  Wt::WApplication *app = new Wt::WApplication(env);
  
  app->setTitle("Hue Light App");

  app->messageResourceBundle().use(app->appRoot() + "strings");
  app->messageResourceBundle().use(app->appRoot() + "templates");

  app->useStyleSheet("css/hangman.css");
  app->useStyleSheet("resource/form.css");


  new HueApp(app->root());

  return app;
}

/** @brief Starting our wt application.
 *
 *  The main function is to simple start our server by creating our wt application
 */
int main(int argc, char **argv)
{
  try {
    Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(Wt::Application, createApplication);

    Session::configureAuth();

    server.run();
  } catch (Wt::WServer::Exception& e) {
    std::cerr << e.what() << std::endl;
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
  }
}
