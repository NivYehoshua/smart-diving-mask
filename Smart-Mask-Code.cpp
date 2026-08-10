#include <LiquidCrystal.h>

// Define LCD interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Global Variables
int depthPlan, PassArdOnBeforeDive, planTime, diveTime;
double metricDepth, speedVertical_M_S;

// Function Prototypes for LCD Overloading
void PrintLcd(String str, int column, int row);
void PrintLcd(int variableI, int column, int row);
void PrintLcd(double variableD, int column, int row);

void setup() {
  // Initialize LCD module (16 columns, 2 rows) and Button pin
  lcd.begin(16, 2);
  pinMode(8, INPUT);
  
  // Set target depth and calculate maximum allowed bottom time
  depthPlan = DiveStart();
  planTime = PlanTime(depthPlan);
  
  // Display the selected depth and allowed time on the LCD
  lcd.clear();
  lcd.print("Decision is ");
  lcd.print(depthPlan);
  lcd.setCursor(0, 1);
  lcd.print("Time is ");
  lcd.print(planTime);
  
  // Display data for 8 seconds before clearing the screen for the dive
  delay(8000);
  lcd.clear();
}

void loop() {
  // Continuous real-time calculations
  metricDepth = MetricDepth();
  diveTime = DiveTime();
  speedVertical_M_S = SpeedVertical_M_S();

  // Print current depth and dive time to the LCD
  PrintLcd(metricDepth, 0, 0);
  PrintLcd(diveTime, 0, 1);
  delay(100);

  // Send data to the safety function for constraint verification
  Safety(metricDepth, speedVertical_M_S, diveTime, planTime);
}

// Function to allow the diver to set target depth via potentiometer
int DiveStart() {
  int depthPlan, buttonSwitch = 8;
  PrintLcd("Enter Max Depth", 0, 0);
  
  // Loop to read potentiometer data until the button is pressed
  while (digitalRead(buttonSwitch) == 0) {
    depthPlan = analogRead(A1) / 890.0 * 42;
    
    // Calibrate maximum depth limit according to decompression tables (42m)
    if (depthPlan > 42) {
      depthPlan = 42;
    }
    
    PrintLcd("Enter Max Depth", 0, 0);
    PrintLcd(depthPlan, 0, 1);
    delay(200);
  }
  
  lcd.clear();
  // Record the timestamp (in seconds) when the dive starts
  PassArdOnBeforeDive = millis() / 1000;
  return depthPlan;
}

// Function to calculate Maximum Bottom Time based on decompression tables
int PlanTime(int temp) {
  if (temp > 0 && temp <= 3) return 300;
  if (temp > 3 && temp <= 4.5) return 350;
  if (temp > 4.5 && temp <= 6) return 325;
  if (temp > 6 && temp <= 7.5) return 315;
  if (temp > 7.5 && temp <= 9) return 310;
  if (temp > 9 && temp <= 10.5) return 270;
  if (temp > 10.5 && temp <= 12) return 170;
  if (temp > 12 && temp <= 15) return 90;
  if (temp > 15 && temp <= 18) return 55;
  if (temp > 18 && temp <= 21) return 45;
  if (temp > 21 && temp <= 24) return 35;
  if (temp > 24 && temp <= 27) return 25;
  if (temp > 27 && temp <= 30) return 22;
  if (temp > 30 && temp <= 33) return 15;
  if (temp > 33 && temp <= 36) return 12;
  if (temp > 36 && temp <= 39) return 8;
  if (temp > 39 && temp <= 42) return 7;
  return 0;
}

// Function to calculate current metric depth using FSR analog signal
double MetricDepth() {
  double v, depth;
  v = analogRead(A0) / 783.0 * 5.0;
  
  // Exponential formula mapped from pressure calibration
  depth = 0.5582 * pow(2.718, 1.0176 * v);
  
  if (v == 0) {
    depth = 0;
  }
  return depth;
}

// Function to calculate vertical speed (ascend/descend rate) in m/s
double SpeedVertical_M_S() {
  double depthMfirst, depthMsecond, verticalDistance, verticalspeed;
  depthMfirst = MetricDepth();
  delay(1000); // 1-second interval for speed calculation
  depthMsecond = MetricDepth();
  verticalDistance = depthMfirst - depthMsecond;
  verticalspeed = verticalDistance / 1.0;
  return verticalspeed;
}

// Function to calculate elapsed dive time in minutes
int DiveTime() {
  int currentDiveTime;
  currentDiveTime = (millis() / 1000) - PassArdOnBeforeDive;
  currentDiveTime = currentDiveTime / 60;
  return currentDiveTime;
}

// Safety protocol: monitor constraints and trigger LCD alerts
void Safety(double metricDepth, double speedVertical_M_S, int diveTime, int planTime) {
  int situation, sitArr[3];
  
  // Constraint 1: Depth > 30m (Narcosis risk)
  if (metricDepth > 30) sitArr[0] = 1; else sitArr[0] = 0;
  
  // Constraint 2: Ascend speed > 10m/min (Decompression risk)
  if (speedVertical_M_S > 1/6.0) sitArr[1] = 1; else sitArr[1] = 0;
  
  // Constraint 3: Dive time exceeds planned time
  if (diveTime >= planTime) sitArr[2] = 1; else sitArr[2] = 0;

  // Convert binary array to a decimal situation state
  situation = BinToInt(sitArr);

  // Handle alerts based on the situation state
  switch (situation) {
    case 0:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      break;
    case 1:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("5MinLeft", 8, 1);
      break;
    case 2:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("TooFast", 9, 0);
      break;
    case 3:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("5MinLeft", 8, 1);
      PrintLcd("TooFast", 9, 0);
      break;
    case 4:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd(">30", 5, 0);
      break;
    case 5:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("5MinLeft", 8, 1);
      PrintLcd(">30", 5, 0);
      break;
    case 6:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("TooFast", 9, 0);
      PrintLcd(">30", 5, 0);
      break;
    case 7:
      lcd.clear();
      PrintLcd(metricDepth, 0, 0);
      PrintLcd(diveTime, 0, 1);
      PrintLcd("5MinLeft", 8, 1);
      PrintLcd("TooFast", 9, 0);
      PrintLcd(">30", 5, 0);
      break;
  }
}

// Helper function: Convert 3-bit binary array to integer
int BinToInt(int bin[]) {
  int indexPow = 0, sum = 0, digit = 0;
  for (int i = 2; i >= 0; i--) {
    if (bin[i] == 1) {
      digit = 1;
      for (int k = 0; k < indexPow; k++) {
        digit = digit * 2;
      }
    } else {
      digit = 0;
    }
    sum = sum + digit;
    indexPow++;
  }
  return sum;
}

// --- LCD Printing Overload Functions ---

void PrintLcd(String str, int column, int row) {
  lcd.setCursor(column, row);
  lcd.print(str);
}

void PrintLcd(int variableI, int column, int row) {
  lcd.setCursor(column, row);
  lcd.print(variableI);
}

void PrintLcd(double variableD, int column, int row) {
  lcd.setCursor(column, row);
  lcd.print(variableD);
}
