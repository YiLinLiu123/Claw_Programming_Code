
/* Purpose: To ensure that the random readings from the sonar is not interfering with the operation with the claw. 
 *  Then take the sonar readings and use it to decide if the claw is stationary and if so, then tell the claw to open or close by the postion of the servo motor arm.
 *  
 *  Approach: I had the sonar collect data and then had the Collector Array to sort through the data such that once it fills with the readings from sonar it will pass
 *            the most recent data that is within a maximum distance (maximum height) to the Manager Array. Once the Manager Array fills, it will check if all the data
 *            is within a certain range of eachother and if it is, the claw is considered "stationary" and the claw will open and close. 
 *  
 */

// defining pins for the sonar

#define GroundPin 10
#define echoPin 11
#define trigPin 12
#define VccPin 13

/*The  following constants are used throughout the program to help control the claw and regulate the sonar
 * UPPERLIMIT: the maximum height from the ground (maximum distance reading) that a reading from the sonar will be used throughout calculations
 * 
 * COLLECTER_SIZE: the size of the collector array, 
 * MANAGER_SIZE: the size of the Manager Array.
 * RANGE_LIMIT: The pre-defined error with the sonar reading, all readings in Manager Array must be within this range. 
 * MAX_TURN: This is put in place to set a maximum angle that the servo turns to.
 *OPEN_CLOSE_SPEED: The speed that regulates how fast the servo turns, essentially regulates the rate at which the claw opens/closes.
 *DELAY: the time that the claw will wait before opening if it already closed or vice-versa, this will give us sometime to get the claw moving so it doesn't immediatly drop 
 *       the object. 
 *
 */
#define UPPERLIMIT 10
#define COLLECTER_SIZE 5
#define MANAGER_SIZE 3
#define RANGE_LIMIT 1
#define MAX_TURN 70
#define OPEN_CLOSE_SPEED 3
#define DELAY 3000 

#define TRUE 1
#define FALSE 0

#define OPEN 1
#define CLOSE 0


#include <Servo.h>
Servo myservo; 

//initiliazing variables


int pos = 0; //pos: position of the servo
double Collecter[COLLECTER_SIZE], Manager[MANAGER_SIZE]; 
int i = 0;
int Collecter_Index = 0;
int Manager_Index = 0;
int difference = 0; // keeping track of the difference between sonar data in the Manager Array
int do_Stuff = TRUE; //do_Stuff: if it is TRUE,the claw will either open or close 
int claw_State = 1; // Descibes the current state of the claw
int distance, duration = 0; //distance from the closest object and the duration that the pulse took for one round trip
  
void setup()
{//setting up the arudino with corresponding pins.
Serial.begin (9600);         
pinMode(VccPin, OUTPUT) ;    
digitalWrite(VccPin, HIGH) ; 
pinMode(echoPin, INPUT);     
pinMode(trigPin, OUTPUT);    
pinMode(GroundPin, OUTPUT) ; 
digitalWrite(GroundPin,LOW) ;

myservo.attach(9); 
myservo.write(90);// initial position of the servo

}



void loop()
{
  sonar();

//The purpose of the following statment was to see if the distance variable is changed correctly so that it only stores values within the predefined range. 
  if (distance >= UPPERLIMIT || distance <= 0)  //deciding whether or not distance is reasonable
  {
      Serial.println("Out of range");      //if not, print the words "Out of range"
  }
  else
  {
      Serial.print(distance);              //printing the value of the variable "distance"
      Serial.println(" cm");               //printing " cm" after the value
  }
  delay(50); //so that the number can be printed on the screen and the program doesn't just 

  if (Collecter_Index < COLLECTER_SIZE) {
    // while the Collector is not full, keep filling it with numbers
      Collecter[Collecter_Index] = distance;
      Collecter_Index++;
    }
  else
    {// collector is full, continue with other operations
      //Serial.println("Run Collector Full");
      Collecter_Index--;  //because of the previous counter_Index++ at the end of the if statement, Counter_Index is same as "size"
     
      if (Manager_Index > MANAGER_SIZE-1)
      {// When the Manager array is full, perform comparison and then start to refill
       do_Stuff = check_Stationary();
        
        if (do_Stuff ==TRUE)
        {
          if (claw_State == OPEN)
          {
            closeClaw();
          }
          else
          {
            openClaw();
          }
        }
        
        Serial.println(" Manager Reset");
        
        for(i = 0; i< MANAGER_SIZE; i++)
        {//fills the manager with "garbage" values to "clear" the array
          Manager[i]= UPPERLIMIT+ 2; 
        }
        Manager_Index = 0; //reset index to re-fill the array
      }
      else
      {// fill the Manager Array if the manager is not full. 
        fillManager();
      }
    }
      
}


void sonar()
{// handling how to get the distance from the sonar, changes the distance variable 
  
  digitalWrite(trigPin, HIGH);           //set trigger pin to HIGH
  delayMicroseconds(1000);               //wait 1000 microseconds
  digitalWrite(trigPin, LOW);            //set trigger pin to LOW
  duration = pulseIn(echoPin, HIGH);     //read echo pin
  distance = (duration/2) / 29.1;        //compute distance from duration of echo Pin
  
}

void openClaw()
{//opening the claw
   Serial.println("OPENNING THE CLAW ");
    for(pos = 180; pos >=MAX_TURN; pos-=OPEN_CLOSE_SPEED)
      {
        myservo.write(pos);
        delay(15);
      }
       claw_State =OPEN ;
       delay(DELAY);// ensure that the claw doesn't close immediatly 
}

void closeClaw()
{//closing the claw
  Serial.println(" CLOSING THE CLAW ");
            for(pos = MAX_TURN; pos <=180; pos+=OPEN_CLOSE_SPEED)
            {
              myservo.write(pos);
              delay(15);
            }
            claw_State = CLOSE;
            delay(DELAY);
}

boolean check_Stationary()
{// checks if the claw is stationary 
   for (i = 0; i < MANAGER_SIZE - 1; i++)
        {// checking if all numbers are within a range of each other and if all numbers are in range
          
          if(i == MANAGER_SIZE-1)
          {
            difference = Manager[i]- Manager[0];
          }else
          {
            difference = Manager[i + 1] - Manager[i];
          }

          if ((difference > RANGE_LIMIT || difference < -RANGE_LIMIT)|| Manager[i]>= UPPERLIMIT)
          { 
            Serial.println("Claw Not Stationary");
            return FALSE;
            break;
          }
          else
          {
            Serial.println("Claw Stationary");
            return TRUE;
            
          }
        }
}

void fillManager()
{ // if Manager Array is not full, transfer value from collecter, collecter is full right

    while (Collecter[Collecter_Index] > UPPERLIMIT && Collecter_Index >= 0)
        {// if the current number (indicated by the index) in the collecter is not within the limit
        //and we can go back to a previous value, do so
          Serial.println(" Run Back Track");
          Collecter_Index--;
        }
        
        // when the while loops exits, either the current value in the collecter is within limit or no value within the Collector array is within range.
        // The "Collecter_Index" could be less than zero, so correct for that. 

        if(Collecter_Index<0)
        {
          Collecter_Index =0;
        }
        if (Collecter[Collecter_Index] < UPPERLIMIT)
        {// I first check if the current index value is within the range and if it is then I pass it onto Manager, Manger is not
          // full right now
          Manager[Manager_Index] = Collecter[Collecter_Index];
         //printing out the collector_Index to make sure it is in the proper position, ie: no -1 or something random like that 
          Serial.println(" RUN FILL Manager");
          Serial.print("Collector_Index ");
          Serial.println(Collecter_Index);
          
          Manager_Index++;
        }

      Collecter_Index = 0;
      Collecter[Collecter_Index] = distance;
      Collecter_Index++;
}


