/* Dowdow Arduino Clone for sleep meditation. 
 *  
 *  Based on https://github.com/gary909/Dodow-Clone
 *  
 *  The purpose of the Dowdow is to progessively slow breathing,
 *  the user breathes in as the light gets brighter and out as
 *  it dims.  Each breath gets slightly longer until the
 *  target respiration rate is acheived.
 *  
 *  The commercial Dowdow device slows the breathing from 11 breaths
 *  per minute to 6 breaths per minute over a period of 8 minutes.
 *  
 *  This clone allows the initial breathing frequency, target breathing 
 *  frequency and the duration of the slow down to be set with three potentiometers.
 *  
 *  Breathing is reduced over "DEFAULT_REDUCING_TIME_PC"% of the selected run time and the
 *  remaining time is constant at the target rate.
 *  
 *  Default frequencies, run-time and extended-time can be set
 *  using the defines at the head of the sketch.
 *  
 *  Wiring
 *  
 *  To use the variable start frequency; a potentiomenter should be
 *  connected with one side to 5V, the other side to GND and the
 *  middle to which ever pin is defined in "INITIAL_FREQUENCY_PIN".  Should
 *  this feature not be required, comment out the #define the pin.
 *  
 *  To use the variable target frequency; a potentiomenter should be
 *  connected with one side to 5V, the other side to GND and the
 *  middle to which ever pin is defined in "TARGET_FREQUENCY_PIN".  Should
 *  this feature not be required, comment out the #define the pin.
 *  
 *  To use the variable run-time; a potentiomenter should be
 *  connected with one side to 5V, the other side to GND and the
 *  middle to which ever pin is defined in "DURATION_PIN".  Should
 *  this feature not be required, comment out the #define the pin.
 *  
 *  A switch can be added to pin "NIGHT_MODE_ENABLE_PIN" which will
 *  reduce the brightness of the LED by a percentage set in the #define
 *  "NIGHT_MODE_PC".  If no switch is to be connected, comment out this #define.
 *  
 *  An LED (with appropriate resistor) should be connected to the pin
 *  set in "LED_PIN" (this must be a PWM output, labelled as ~ on most
 *  Arduino boards).  If using more than one LED for brightness then connect 
 *  this pin to a transistor or mosfet and connect the LEDs to that.
 *  
 */

 //Include Libraries

#if defined(__AVR__)
  #include <avr/sleep.h>
#endif
  
//Set up options

#define DEFAULT_INITIAL_FREQUENCY 11  //breaths per minute to start from
#define DEFAULT_TARGET_FREQUENCY 6    //breaths per minute to end at
#define DEFAULT_DURATION 8            //the breathing will be slowed over this number of minutes
#define DEFAULT_REDUCING_TIME_PC 80   //the device will continue at the target rate for this number of seconds

#define LED_PIN 6                     //The output pin (connect LED here or base of transistor/ gate of mosfet if driving more than the arduino can manage).
#define INITIAL_FREQUENCY_PIN 14      //Analog input pin for setting of initial frequency. [14 = A0]
#define TARGET_FREQUENCY_PIN 15       //Analog input pin for setting of target frequency. [15 = A1]
#define DURATION_PIN 16               //Analog input pin for setting of duration. [16 = A2]
#define NIGHT_MODE_ENABLE_PIN 2       //Digital pin for setting whether to run period of extra steady time at the end.

#if defined(__AVR__)
  #define PWM_MAX 255                 //Arduino has 8-bit PWM
#elif defined(ESP8266)
  #define PWM_MAX 1023                //ESP8266 has 10-bit PWM
#endif

#define GAP_BRIGHTNESS_PC 10          //Percentage of PWN_RANGE value for dimmest setting (gap between breaths)
#define NIGHT_MODE_PC 50              //Percentage by which to lower brightness if night mode switch enabled.
#define INPUT_MAX 1023                //Range of analogue inputs.

#define MIN_INITIAL_FREQUENCY 10      //Minimum breaths per second the device can be set to start at
#define MAX_INITIAL_FREQUENCY 18      //Maximum breaths per second the device can be set to start at
#define MIN_TARGET_FREQUENCY 5        //Minimum breaths per second the device can be set to finish at
#define MAX_TARGET_FREQUENCY 9        //Maximum breaths per second the device can be set to finish at
#define MIN_DURATION 5                //Minimum duration over which the device will be allowed to work (minutes)
#define MAX_DURATION 20               //Maximum duration over which the device will be allowed to work (minutes)

//#define DEBUG 1                     //Uncommenting this line sends debug info over serial.

//Fade Function

void fadeLED(int pin, int duration_of_fade, int low_value, int high_value) {
  int period_time = duration_of_fade / 3;
  int delay_time = (period_time * 2)/ (high_value - low_value);
  for (int i=(low_value - 1); i<high_value; i++) {
    analogWrite(pin, i);
    delay(delay_time);
  }
  for (int j=(high_value - 1); j>low_value; j--) {
    analogWrite(pin, j);
    delay(delay_time);
  }
  delay(period_time);
}


void realBreatheLED(int pin, int duration_of_fade, int low_value, int high_value) {
  int period_time = duration_of_fade / 5;
  int up_delay_time = period_time/ (high_value - low_value);
  int down_delay_time = (period_time * 2)/ (high_value - low_value);
  for (int i=(low_value - 1); i<high_value; i++) {
    analogWrite(pin, i);
    delay(up)delay_time);
  }
  for (int j=(high_value - 1); j>low_value; j--) {
    analogWrite(pin, j);
    delay(down_delay_time);
  }
  delay(period_time * 2);
}

void setup() {

  //Everything happens here, the loop never runs because after once through the code we go to sleep.
  //On reset this runs again.

  //Set up the output pin
  pinMode(LED_PIN, OUTPUT);

  //Decide if variable initial frequency is in play and set it accordingly.
  #ifdef INITIAL_FREQUENCY_PIN
    pinMode(INITIAL_FREQUENCY_PIN, INPUT);
    float initial_bpm = map(analogRead(INITIAL_FREQUENCY_PIN), 0, INPUT_MAX, (MIN_INITIAL_FREQUENCY * 60), (MAX_INITIAL_FREQUENCY * 60)) / 60.0;
  #else
    float initial_bpm = DEFAULT_INITIAL_FREQUENCY;
  #endif
  
  //Decide if variable target frequency is in play and set it accordingly.
  #ifdef TARGET_FREQUENCY_PIN
    pinMode(TARGET_FREQUENCY_PIN, INPUT);
    float target_bpm = map(analogRead(TARGET_FREQUENCY_PIN), 0, INPUT_MAX, (MIN_TARGET_FREQUENCY * 60), (MAX_TARGET_FREQUENCY * 60)) / 60.0;
  #else
    float target_bpm = DEFAULT_TARGET_FREQUENCY;
  #endif

  //Decide if variable duration is in play and set it accordingly.
  #ifdef DURATION_PIN
    pinMode(DURATION_PIN, INPUT);
    float duration = map(analogRead(DURATION_PIN), 0, INPUT_MAX, (MIN_DURATION * 60), (MAX_DURATION * 60));
  #else
    float duration = DEFAULT_DURATION * 60.0;
  #endif

  //Check if there is a switch for night mode
  #ifdef NIGHT_MODE_ENABLE_PIN
    pinMode(NIGHT_MODE_ENABLE_PIN, INPUT);
    int max_brightness = PWM_MAX;
    if (digitalRead(NIGHT_MODE_ENABLE_PIN)) {
      max_brightness = max_brightness * (NIGHT_MODE_PC / 100.0);
    }
  #else
    int max_brightness = PWM_MAX;
  #endif

  //Set up some maths to calculate how quickly to change the delay in each fade.

  float reducing_duration = duration * (DEFAULT_REDUCING_TIME_PC / 100.0);
  float steady_duration = duration - reducing_duration;
  
  float initial_duration_ms = (60 / initial_bpm) * 1000;
  float target_duration_ms = (60 / target_bpm) * 1000;

  float average_duration_ms = (initial_duration_ms + target_duration_ms) / 2;
  float number_of_reducing_breaths = reducing_duration / (average_duration_ms / 1000);
  
  float change_in_duration_ms = round((target_duration_ms - initial_duration_ms) / number_of_reducing_breaths);

  int min_brightness = max_brightness * (GAP_BRIGHTNESS_PC / 100.0);

  //If the debug flag is enabled, output data on the console
  #ifdef DEBUG
    float total_duration = 0;
    Serial.begin(9600);
    Serial.println("DowDow Arduino Clone");
    Serial.println("====================");
    Serial.println();
    Serial.println("Input values");
    Serial.println("------------");
    Serial.println();
    #ifdef INITIAL_FREQUENCY_PIN
      Serial.print("Initial Frequency Input: ");
      Serial.print(analogRead(INITIAL_FREQUENCY_PIN));
      Serial.print("/");
      Serial.println(INPUT_MAX);
    #endif
    #ifdef TARGET_FREQUENCY_PIN
      Serial.print("Target Frequency Input: ");
      Serial.print(analogRead(TARGET_FREQUENCY_PIN));
      Serial.print("/");
      Serial.println(INPUT_MAX);
    #endif
    #ifdef DURATION_PIN
      Serial.print("Duration Input: ");
      Serial.print(analogRead(DURATION_PIN));
      Serial.print("/");
      Serial.println(INPUT_MAX);
    #endif
    #ifdef NIGHT_MODE_ENABLE_PIN
      Serial.print("Night Mode Enabe: ");
      Serial.println(digitalRead(NIGHT_MODE_ENABLE_PIN));
    #endif
    Serial.println();
    Serial.println("Initial Settings");
    Serial.println("----------------");
    Serial.println();
    Serial.print("Initial Frequency (bpm): ");
    Serial.println(initial_bpm);
    Serial.print("Target Frequency (bpm): ");
    Serial.println(target_bpm);
    Serial.print("Duration (minutes): ");
    Serial.println(duration / 60);
    Serial.print("Duration (seconds): ");
    Serial.println(duration);
    Serial.println();
    Serial.println("Calculated Settings");
    Serial.println("-------------------");
    Serial.println();
    float reduction_to_stable_fraction = DEFAULT_REDUCING_TIME_PC / 100.0;
    Serial.print("Reduction to Stable Ratio: ");
    Serial.println(reduction_to_stable_fraction);
    Serial.print("Reducing duration (s): ");
    Serial.println(reducing_duration);
    Serial.print("Steady duration (s): ");
    Serial.println(steady_duration);
    Serial.print("Initial breath duration (ms): ");
    Serial.println(initial_duration_ms);
    Serial.print("Target breath duration (ms): ");
    Serial.println(target_duration_ms);
    Serial.print("Average breath duration (ms): ");
    Serial.println(average_duration_ms);
    Serial.print("Total number of reducing breaths required: ");
    Serial.println(number_of_reducing_breaths);
    Serial.print("Breath duration delta (ms): ");
    Serial.println(change_in_duration_ms);
    float min_brightness_fraction = (GAP_BRIGHTNESS_PC / 100.0);
    Serial.print("Min to Max Brightness Ratio: ");
    Serial.println(min_brightness_fraction);
    Serial.print("Max Brightness: ");
    Serial.println(max_brightness);
    Serial.print("Min Brightness: ");
    Serial.println(min_brightness);
    Serial.println();
    Serial.print("Extra time duration (s): ");
    Serial.println(steady_duration);
    Serial.println();
    Serial.println("Running now");
    Serial.println("-----------");
    Serial.println();
  #endif

  //We're done seting up, lets start running

  int this_duration = initial_duration_ms;
  
  while (this_duration <= target_duration_ms) {
    //If the debug flag is enabled, output data on the console
    #ifdef DEBUG
      Serial.print("This duration: ");
      Serial.print(this_duration);
      Serial.println("ms.");
      total_duration = total_duration + this_duration;
    #endif
   realBreatheLED(LED_PIN, this_duration, min_brightness, max_brightness);
   this_duration = this_duration + change_in_duration_ms;
  }

  //If the debug flag is enabled, output data on the console
  #ifdef DEBUG
    Serial.println();
    Serial.println("Run period stats");
    Serial.println("----------------");
    Serial.println();
    Serial.print("Total reducing period was: ");
    Serial.print(total_duration);
    Serial.print(" ms = ");
    Serial.print(total_duration/1000);
    Serial.print(" s = ");
    Serial.print(total_duration/60000);
    Serial.println(" minutes.");
  #endif

  //Run the extra piece of stable rate breathing at the end...
  float steady_time_run = 0;
  while (steady_time_run<steady_duration) {
    realBreatheLED(LED_PIN, target_duration_ms, min_brightness, max_brightness);
    steady_time_run = steady_time_run + (target_duration_ms / 1000);
  }

  //If the debug flag is enabled, output data on the console
  #ifdef DEBUG
    Serial.print("Steady time run: ");
    Serial.print(steady_time_run);
    Serial.print(" s = ");
    Serial.print(steady_time_run / 60);
    Serial.println(" minutes.");
    Serial.println();
    float total_duration_run = (total_duration/1000) + steady_time_run;
    Serial.print("Total running time: ");
    Serial.print(total_duration_run);
    Serial.print(" s = ");
    Serial.print(total_duration_run / 60);
    Serial.println(" minutes.");
    Serial.println("Thank you, going to sleep now...");
  #endif

  //Shut everything off
  analogWrite(LED_PIN, 0);
  digitalWrite(LED_PIN, LOW);

  //Go to sleep
  #if defined(__AVR__)
    set_sleep_mode (SLEEP_MODE_PWR_DOWN); 
    sleep_enable();
    sleep_cpu ();  
  #elif defined(ESP8266)
    ESP.deepSleep(0);
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:

}
