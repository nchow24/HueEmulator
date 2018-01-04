#ifndef AUTH_WIDGET_H_
#define AUTH_WIDGET_H_

#include <Wt/Auth/AuthWidget>

class Session;

class AuthWidget : public Wt::Auth::AuthWidget
{
public:
  AuthWidget(Session& session);

  /* We will use a custom registration view */
  virtual Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);

private:
  Session& session_;
};

#endif // AUTH_WIDGET_H_
