#ifndef Cameraslide_h
#define Cameraslide_h

#include "Arduino.h"

class BasicStepperDriver;

class Cameraslide
{
  public:
    Cameraslide();
    void setMotorSteps(int stepsPerRevolution);
    void setMotorRevolutions(int revolutionsPerCycle);
    void setStepPin(int stepPin);
    void setDirectionPin(int directionPin);
    void setEndSwitches(int startSwitchPin, int endSwitchPin);
    void setCycleTime(int hours, int minutes, int seconds);
    void setDirection(boolean dir);
    void setShutterSpeed(int shutterSpeed);
    void setPictureTimer(int milliseconds);
    void setFocusTimer(int milliseconds);
    void setInterval(int minutes, int seconds);
    void setPauseBeforePicture(double seconds);
    void setPauseAfterPicture(double seconds);
    void setNonStop(boolean nonStopBit);
    void setTakePicPin(int pin);
    void setFocusPin(int pin);
    void start();

  private:
    int _motorSteps = 200 / 10; //actual motor steps / 10 to go below 1 rpm (this is because of restrictions in stepper library)
    unsigned long _motorRevolutions = 22;
    int _totalStepsPerCycle;
    int _stepPin = 10;
    int _directionPin = 11;
    int _enablePin = 12;
    double _motorRPM;
    int _startSwitch;
    int _endSwitch;
    int _hours = 0;
    int _minutes = 10;
    int _seconds = 0;
    boolean _direction = 1;
    double _shutterSpeed = 1;
    double _pauseBeforePicture = 2;
    double _pauseAfterPicture;
    double _totalPauseTime;
    long _cycleTime;
    unsigned long _totalPauseTimePerCycle;
    int _interval = 5;
    unsigned long _totalDriveTimePerCycle;
    unsigned long _currentMillis;
    boolean _nonStop = 0;
    unsigned long _lastPicMillis;
    double _focusTimer = 500;
    double _pictureTimer = 200; //ms to push down camera button
    int _takePicPin;
    int _focusPin;
    unsigned long _driveTime;
	boolean _parametersOk;
    
	BasicStepperDriver* stepper;

    void checkPictureTimer();
    void takePicture();
	void updateParameters();
};


#endif

