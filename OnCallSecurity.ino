#include <mxapi.h>
#include <lcd.h>
#include <ms_delay.h>
#include <sound.h>
#include <ms_analog.h>

boolean alarm = false; //variable for emergency
boolean armed = false; //variable for armed/disarmed state
boolean trouble = false; //variable to detect system faults (motion, doors/windows)
boolean code = false; //variable to detect if code was entered correctly
int analog_value; //temp variable
int light_sensor_val; //light sensor variable

/*---Alarm Key----*/
boolean one = false;
boolean three = false;
boolean seven = false;
boolean nine = false;

int main(void)
{
    /*--------- INITIALIZE KEYPAD -------------*/
    UCSR0B=0;   //Disable USART communication on Ports D0 and D1

    /* Set up Ports D0 to D3 as inputs */
    input_pin(PORT_D0);
    input_pin(PORT_D1);
    input_pin(PORT_D2);
    input_pin(PORT_D3);

    /* Activate internal pull-up resistors */
    pullup_on(PORT_D0);
    pullup_on(PORT_D1);
    pullup_on(PORT_D2);
    pullup_on(PORT_D3);

    /* Set up Ports D4 to D7 as outputs */
    output_pin(PORT_D4);
    output_pin(PORT_D5);
    output_pin(PORT_D6);
    output_pin(PORT_D7); 

    /*Set Ports D4 to D7 high to start */
    high_pin(PORT_D4);
    high_pin(PORT_D5);
    high_pin(PORT_D6);
    high_pin(PORT_D7);

    lcd_init(); //Initialize LCD

    adc_init();  //Initialize Temp Sensor
    
    output_pin(PORT_B5);   //Set up Port B0 as an output pin (Speaker)
    output_pin(PORT_B0);   //Set up Port B0 as an output pin (Red LED)
    input_pin(PORT_C5);   //Set up Port C5 (light sensor) as an input 
    
    /*-------Begin Active Security Coding---------------*/
    while(1==1)
    {
      /*---------Define States of System---------------*/
      if(trouble == false && armed == false)  //System Normal
      {
        low_pin(PORT_B0);     //Set the Port B0 pin low (Red LED OFF)
        high_pin(PORT_B5);  //turn off alarm
        lcd_clear();
        lcd_text(FIRST_LINE, "System Ready");
        trouble = checktrouble(); //Check to see if there is trouble
        checkTemp();
      }
      else if(trouble == true && armed == false) //Non-Critical Trouble (sensor tripped but system unarmed; system cannot be armed in this state)
      {
        lcd_clear();
        lcd_text(FIRST_LINE, "TROUBLE");
        high_pin(PORT_B0);     //Set the Port B0 pin high (Red LED ON)
        trouble = checktrouble(); //Check to see if there is still trouble
        checkTemp();
      }
      else if(trouble == false && armed == true) //System is Armed; No Trouble Detected
      {
        lcd_clear();
        lcd_text(FIRST_LINE, "ARMED");
        while(trouble == false)
        {
          high_pin(PORT_B0);     //Set the Port B0 pin low (Red LED ON)
          delay_ms(500);        //Wait for 500 milliseconds
          trouble = checktrouble();  //Check if there is trouble
          checkTemp();
          low_pin(PORT_B0);     //Set the Port B0 pin low (Red LED OFF)
          delay_ms(500);        //Wait for 500 milliseconds
          trouble = checktrouble();  //Check if there is trouble
          checkTemp();
        }
      }
      else if(trouble == true && armed == true) //System is Armed; Trouble Detected
      {
        high_pin(PORT_B0);     //Set the Port B0 pin high (Red LED ON)
        lcd_clear();
        lcd_text(FIRST_LINE, "ALARM");
        lcd_text(SECOND_LINE, "Enter Code:");
        armed = checkCode();
      }
      
      /*----------------Arm System----------------*/
      low_pin(PORT_D4);
      if(pin_value(PORT_D3) == 0)
      {
         if(trouble == false && armed == false)  //ARM SYSTEM
         {
           low_pin(PORT_B0);     //Set the Port B0 pin low (Red LED OFF)
           one = false;
           three = false;
           seven = false;
           nine = false;
           code = false;
           lcd_clear();
           lcd_text(FIRST_LINE, "Arming...");
           lcd_text(SECOND_LINE, "Exit Within 10 sec");
           delay_ms(10000);        //Wait for 10 seconds
           armed = true;
         }
       }
       high_pin(PORT_D4);
  }
}

/*------------------Monitors Number Buttons---------------------*/
boolean numscan(void)
{
   while(1==1)
   {
      low_pin(PORT_D4);          //Scan row 1 (123A)
      if(pin_value(PORT_D0) == 0)
      {
         one = true;
         three = false;
         seven = false;
         nine = false;
      }
      if(pin_value(PORT_D1) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      if(pin_value(PORT_D2) == 0)
      {
         if(one)
         {
           three = true;
         } else{
           one = false;
           three = false;
           seven = false;
           nine = false;
         }
      }
      high_pin(PORT_D4);

      low_pin(PORT_D5);   //Scan row 2 (456B)
      if(pin_value(PORT_D0) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      if(pin_value(PORT_D1) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      if(pin_value(PORT_D2) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      high_pin(PORT_D5);

      low_pin(PORT_D6);   //Scan row 3 (789C)         
      if(pin_value(PORT_D0) == 0)
      {
         if(three)
         {
           seven = true;
         } else{
           one = false;
           three = false;
           seven = false;
           nine = false;
         }
      }
      if(pin_value(PORT_D1) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      if(pin_value(PORT_D2) == 0)
      {
         if(seven)
         {
           nine = true;
         } else {
           one = false;
           three = false;
           seven = false;
           nine = false;
         }
      } 
      high_pin(PORT_D6);

      low_pin(PORT_D7);   //Scan row 4 (0FED)
      if(pin_value(PORT_D0) == 0)
      {
         one = false;
         three = false;
         seven = false;
         nine = false;
      }
      high_pin(PORT_D7);
      return(nine); //Return the value of the key (number) pressed
   }
}

boolean checkCode(void)
{
  int counter = 0;
  while(counter<=10000 && code==false) //continuously check for 10sec
  {
    code=numscan();
    delay_ms(1);        //Wait for 1 mseconds
    counter++;
  }
  while(!code)
  {
    code=numscan();
    tone_out(PORT_B5, 440, 200);  // Make a 440-Hertz tone for 200 milliseconds
    tone_out(PORT_B5, 535, 100);  // Make a 535-Hertz tone for 100 milliseconds
  }
  return(false); //turn off alarm
}

boolean checktrouble(void)
{
  light_sensor_val=adc_read(5);         //Read ADC value for Port C5
  low_pin(PORT_D7);   //Scan row 4 (0FED)
  if(pin_value(PORT_D3) == 0) //Door/Window Sensor
  {
    return(true);
  }
  else if(light_sensor_val>1020)              //If it gets dark 
  { 
    return(true); 
  }
  else {
    return(false);
  }
  high_pin(PORT_D7);
}

void checkTemp(void)
{
  analog_value=adc_read(4)/2;     //Read Temperature value on Port C4
  if(analog_value < 10 || analog_value > 27) //if temp is not between 10 and 30 degees C
  {
    high_pin(PORT_B0);     //Set the Port B0 pin high (Red LED ON)
    lcd_clear();
    lcd_text(FIRST_LINE, "TEMP ALARM");
    lcd_text(SECOND_LINE, "Enter Code:");
    armed = checkCode();
  }
}
