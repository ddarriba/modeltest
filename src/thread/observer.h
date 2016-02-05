#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>

class Observable;

class Observer
{
  public:
    Observer();
    virtual ~Observer();
    virtual void update(Observable * subject) = 0;
};

class Observable
{
    std::vector < Observer * > views;
  public:
    virtual ~Observable();
    void attach(Observer *obs);
    virtual void notify();
};

#endif // OBSERVER_H
