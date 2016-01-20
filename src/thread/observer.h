#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
using namespace std;

class Observable;

class Observer
{
  public:
    Observer()
    {
    }
    virtual void update(Observable * subject) = 0;
};

class Observable
{
    vector < Observer * > views;
    int value;
  public:
    void attach(Observer *obs)
    {
        views.push_back(obs);
    }
    virtual void notify()
    {
        // publisher broadcasts
        for (Observer * view : views)
            view->update(this);
    }
};

#endif // OBSERVER_H
