#include "observer.h"

Observer::Observer() {}

Observer::~Observer() {}

Observable::~Observable() {}

void Observable::attach(Observer *obs)
{
    views.push_back(obs);
}

void Observable::notify()
{
    // publisher broadcasts
    for (Observer * view : views)
        view->update(this);
}
