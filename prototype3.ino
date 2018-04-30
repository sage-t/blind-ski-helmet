#include <Servo.h>

// settings
const int minRange = 69;
const int maxRange = 131;
const int center = 100;
const int centerHalfRange = 3;
const int stepSize = 2;
const int threshold = 100; // min range (cm) to warn user of
const int cycleLength = 5; // length of vibration
const int cycleDelay = 20;
int currCycle = 0;
bool plus = true;

// vibration bands
const int rightBandPin = 10;
const int leftBandPin = 12;

// servo
Servo sweeper;
int pos = 0;

// proximity sensor
const int proxPin = 0;
int distance = 100000;

void setup() {
  Serial.begin(9600);

  sweeper.attach(6);

  pinMode(rightBandPin, OUTPUT);
  pinMode(leftBandPin, OUTPUT);
}

int getDistance() {
  distance = analogRead(proxPin) * 0.5;
  
  Serial.print("\nDistance in centimeters: ");
  Serial.print(distance);
  
  return distance;
}


bool isCenterPos() {
  return pos >= center - centerHalfRange && pos <= center + centerHalfRange;
}

bool isLeftPos() {
  return pos < center - centerHalfRange;
}

bool isRightPos() {
  return pos > center + centerHalfRange;
}


void vibrateBands() {
  if (distance > threshold) {
    currCycle--;

    if (currCycle < 1) {
      digitalWrite(rightBandPin, LOW);
      digitalWrite(leftBandPin, LOW);
    }
     
    return;
  }

  bool rightBand = false;
  bool leftBand = false;
  if (isLeftPos()) {
    leftBand = true;
  } else if (isCenterPos()) {
    leftBand = true;
    rightBand = true;
  } else if (isRightPos()) {
    rightBand = true;
  }

  if (rightBand || leftBand) {
    currCycle = cycleLength;
  }

  if (rightBand && leftBand) {
    digitalWrite(rightBandPin, HIGH);
    digitalWrite(leftBandPin, HIGH);
  } else if (rightBand) {
    digitalWrite(rightBandPin, HIGH);
    digitalWrite(leftBandPin, LOW);
  } else if (leftBand) {
    digitalWrite(leftBandPin, HIGH);
    digitalWrite(rightBandPin, LOW);
  }

}

void loop() {

  /* lock sensor in center if object is detected there */
  if (!isCenterPos() || distance >= threshold) {
    if (plus) {
      pos += stepSize;
    } else {
      pos -= stepSize;
    }
  
    if (pos >= maxRange) {
      plus = false;
    } else if (pos <= minRange) {
      plus = true;
    }
  }
  
  sweeper.write(pos);
  delay(cycleDelay);

  if (distance < threshold) {
    distance = getDistance();
  } else {
    distance = getDistance();
    /* double check if object is detected (filter out false positives) */
    for (int i = 0; i < 5; i++) {
      if (distance < threshold) {
        distance = getDistance();
      } else {
        break;
      }
    }
  }
  
  vibrateBands();
}
