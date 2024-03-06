//TimerSystemClass.cpp

#include "TimerSystemClass.h"

TIMERSYSTEMCLASS::TIMERSYSTEMCLASS(int customDelay)
{
  startingPoint = 0;
  setDelay = customDelay;
  counting = false;

}

void TIMERSYSTEMCLASS::miliCurrent(){
  if (startingPoint == 0)
  {
    startingPoint = millis();

  }
  counting = millis() - startingPoint >= setDelay;




}

void TIMERSYSTEMCLASS::reset()
{
  counting = false;
  start = 0;
}
void TIMERSYSTEMCLASS::isBusy()
{
  return counting;
}

