#include <Arduino.h>
#include "..\src\BasicStepperDriver.h"
#include "Cameraslide.h"


Cameraslide::Cameraslide()
{
  _motorSteps = 200 / 10; //actual motor steps / 10 to go below 1 rpm (this is because of restrictions in stepper library)
  _motorRevolutions = 22;

  _stepPin = 10;
  _directionPin = 11;
  _enablePin = 12;
  
  _startSwitch = 6;
  _endSwitch = 7;
  _hours = 0;
  _minutes = 10;
  _seconds = 0;
  _direction = 1;
  _shutterSpeed = 1;
  _pauseBeforePicture = 2;
  _pauseAfterPicture = 0.5;
  
  _cycleTime = 600;
  
  _interval = 10;
  
  
  _nonStop = 0;
  
  _focusTimer = 100;
  _pictureTimer = 100; //ms to push down camera button
  _takePicPin = 13;
  _focusPin = 9;


  _totalStepsPerCycle = _motorSteps * _motorRevolutions;
  _totalPauseTime = _pauseBeforePicture + _shutterSpeed + _pauseAfterPicture + (_focusTimer * 0.001) + (_pictureTimer * 0.001);
  _totalPauseTimePerCycle = (_cycleTime / _interval) * _totalPauseTime;
  _totalDriveTimePerCycle = _cycleTime - _totalPauseTimePerCycle;
  _motorRPM = _motorRevolutions / (_totalDriveTimePerCycle / 60.0);

	stepper = new BasicStepperDriver(_motorSteps, _directionPin, _stepPin, _enablePin);
  stepper->setMicrostep(1); //Full step
  
  pinMode(_startSwitch, INPUT);
  pinMode(_endSwitch, INPUT);
  pinMode(_takePicPin, OUTPUT);
  pinMode(_focusPin, OUTPUT);
  
  _parametersOk = 1;
}

void Cameraslide::updateParameters() {
	_totalStepsPerCycle = _motorSteps * _motorRevolutions;
	_totalPauseTime = _pauseBeforePicture + _shutterSpeed + _pauseAfterPicture + (_focusTimer * 0.001) + (_pictureTimer * 0.001);
	_totalPauseTimePerCycle = (_cycleTime / _interval) * _totalPauseTime;
	_totalDriveTimePerCycle = _cycleTime - _totalPauseTimePerCycle;
	_motorRPM = _motorRevolutions / (_totalDriveTimePerCycle / 60.0);
	
	if (_totalPauseTimePerCycle >= _cycleTime || _motorRPM > 200 * 10) {
		Serial.println("Cycle time is too short for specified interval. Try again using a longer cycle time");
		_parametersOk = 0;
	}
	else {
	_parametersOk = 1;
	}
}

void Cameraslide::setMotorSteps(int stepsPerRevolution) {
  _motorSteps = stepsPerRevolution / 10;
  Serial.print("Motor steps per revolution: ");
  Serial.println(_motorSteps);
  updateParameters();
}

void Cameraslide::setMotorRevolutions(int revolutionsPerCycle) {
  _motorRevolutions = revolutionsPerCycle;
  updateParameters();
  Serial.print("Motor revolutions per cycle: ");
  Serial.println(_motorRevolutions);
}

void Cameraslide::setStepPin(int stepPin) {
  _stepPin = stepPin;
  pinMode(_stepPin, OUTPUT);
  Serial.print("Step pin on pin: ");
  Serial.println(_stepPin);
}

void Cameraslide::setDirectionPin(int directionPin) {
  _directionPin = directionPin;
  pinMode(_directionPin, OUTPUT);
  Serial.print("Direction pin on pin: ");
  Serial.println(_directionPin);
}

void Cameraslide::setEndSwitches(int startSwitchPin, int endSwitchPin) {
  _startSwitch = startSwitchPin;
  _endSwitch = endSwitchPin;
  pinMode(_startSwitch, INPUT);
  pinMode(_endSwitch, INPUT);
  Serial.print("Start switch on pin: ");
  Serial.println(_startSwitch);
  Serial.print("End switch on pin: ");
  Serial.println(_endSwitch);
}

void Cameraslide::setCycleTime(int hours, int minutes, int seconds) {
  _cycleTime = (hours * 60 * 60) + (minutes * 60) + seconds;
  updateParameters();
  Serial.print("Cycle time 's' : ");
  Serial.println(_cycleTime);
}

void Cameraslide::setDirection(boolean dir) {
  _direction = dir;
  Serial.print("Motor direction: "); 
  if (_direction == 1) {
    digitalWrite(_directionPin, HIGH);
	Serial.println("CW");
  }
  else {
    digitalWrite(_directionPin, LOW);
	Serial.print("CCW");
  }
  
}

void Cameraslide::setShutterSpeed(int shutterSpeed) {
  _shutterSpeed = shutterSpeed;
  updateParameters();
  Serial.print("Shutter speed 'ms': ");
  Serial.println(_shutterSpeed);
}

void Cameraslide::setPictureTimer(int milliseconds) {
  _pictureTimer = milliseconds;
  Serial.print("Picture timer 'ms': ");
  Serial.println(_pictureTimer);
  updateParameters();
}

void Cameraslide::setFocusTimer(int milliseconds) {
  _focusTimer = milliseconds;
  Serial.print("Focus timer 'ms': ");
  Serial.println(_focusTimer);
  updateParameters();
}

void Cameraslide::setInterval(int minutes, int seconds) {
  _interval = (minutes * 60) + seconds;
  //warning if shutter speed * 2 > interval
  if (_interval < _shutterSpeed * 2) {
    _interval = (_shutterSpeed * 2) + 1;
  }
  updateParameters();
  Serial.print("Interval 's': ");
  Serial.println(_interval);
}

void Cameraslide::setPauseBeforePicture(double seconds) {
  _pauseBeforePicture = seconds;
  updateParameters();
  Serial.print("Pause before picture 's': ");
  Serial.println(_pauseBeforePicture);
}

void Cameraslide::setPauseAfterPicture(double seconds) {
  _pauseAfterPicture = seconds;
  updateParameters();
  Serial.print("Pause after picture 's': ");
  Serial.println(_pauseAfterPicture);
}

void Cameraslide::setNonStop(boolean nonStopBit) {
  _nonStop = nonStopBit;
}

void Cameraslide::setTakePicPin(int pin) {
  _takePicPin = pin;
  pinMode(_takePicPin, OUTPUT);
  Serial.print("Take pic pin on pin: ");
  Serial.println(_takePicPin);
}

void Cameraslide::setFocusPin(int pin) {
  _focusPin = pin;
  pinMode(_focusPin, OUTPUT);
  Serial.print("Focus pin on pin: ");
  Serial.println(_focusPin);
}

void Cameraslide::checkPictureTimer() {
  if (digitalRead(_focusPin)) {
    if (_lastPicMillis + _focusTimer < millis()) {
      if (!digitalRead(_takePicPin)) {
        Serial.println("picture");
      }
      digitalWrite(_takePicPin, HIGH);
    }
  }
  if (_lastPicMillis + _focusTimer + _pictureTimer < millis()) {
    digitalWrite(_takePicPin, LOW);
    digitalWrite(_focusPin, LOW);
    //Serial.println("done");
  }
}

void Cameraslide::takePicture() {
  Serial.println("focus");
  //int timer = 2000;
  //_currentMillis = millis();
  _lastPicMillis = millis();
  //while (_currentMillis + timer > millis()) {
  digitalWrite(_focusPin, HIGH);
  //}
  //digitalWrite(13, LOW);
  //Serial.println("picture slut");
  _currentMillis = millis();

  while (digitalRead(_focusPin)) {
    checkPictureTimer();
  }
}

void Cameraslide::start() {
  if (!_parametersOk) {
  return;
  }
  
  Serial.println("Starting time lapse session.");
  Serial.println();
  Serial.println("Parameters");
  Serial.print("Session time 's': ");
  Serial.println(_cycleTime);
  Serial.print("Photo interval 's': ");
  Serial.println(_interval);
  Serial.print("Total time standing still 's': ");
  Serial.println(_totalPauseTimePerCycle);
  Serial.print("Total time driving 's': ");
  Serial.println(_totalDriveTimePerCycle);
  Serial.print("Motor rpm: ");
  Serial.println(_motorRPM / 10);
  Serial.println();
  
  stepper->setRPM((int)(_motorRPM * 10)); //Math to go below 1 rpm
int counter = 0;
  int revolutions = 0;
  Serial.println("start");
  delay(5000);

  while (!digitalRead(_startSwitch) && !digitalRead(_endSwitch)) {
    _currentMillis = millis();

    while (_currentMillis + (_pauseBeforePicture * 1000) > millis()) {
    }

    takePicture();
    _currentMillis = millis();

    while (_currentMillis + (_pauseAfterPicture * 1000) > millis()) {
    }

    while (_currentMillis + (_interval * 1000) > millis() && !digitalRead(_startSwitch) && !digitalRead(_endSwitch)) {
      stepper->enable();
      Serial.print("move ");
      Serial.println(_motorRPM);
      stepper->move(1);
      counter++;
      if (counter == 200) {
        counter = 0;
        revolutions++;
        Serial.print("Revolutions: ");
        Serial.println(revolutions);
      }
    }
    stepper->disable();
    Serial.println("disable");
  }

  _currentMillis = millis();
  Serial.println("start slut");
}
