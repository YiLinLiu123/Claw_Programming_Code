#define GroundPin 10
#define echoPin 11
#define trigPin 12
#define VccPin 13
#define UPPERLIMIT 10
#define COLLECTER_SIZE 5
#define MANAGER_SIZE 3
#define RANGE_LIMIT 1
#define MAX_TURN 70
#define OPEN_NOW_LIMIT 5
#define OPEN_CLOSE_SPEED 3
#define DELAY 3000 
// woah there
#include <Servo.h>
Servo myservo; 
int pos = 0;

double Collecter[COLLECTER_SIZE], Manager[MANAGER_SIZE];
int i = 0;
int Collecter_Index = 0;
int Manager_Index = 0;
int difference = 0;
int do_Stuff = 1;
int open_Close = 1;
  //1 is for close 0 is for open 

void setup()
{
Serial.begin (9600);         
pinMode(VccPin, OUTPUT) ;    
digitalWrite(VccPin, HIGH) ; 
pinMode(echoPin, INPUT);     
pinMode(trigPin, OUTPUT);    
pinMode(GroundPin, OUTPUT) ; 
digitalWrite(GroundPin,LOW) ;

myservo.attach(9); 
myservo.write(90);

}



void loop()
{
int duration, distance;                //defining variables
digitalWrite(trigPin, HIGH);           //set trigger pin to HIGH
delayMicroseconds(1000);               //wait 1000 microseconds
digitalWrite(trigPin, LOW);            //set trigger pin to LOW
duration = pulseIn(echoPin, HIGH);     //read echo pin
distance = (duration/2) / 29.1;        //compute distance from duration of echo Pin
if (distance >= UPPERLIMIT || distance <= 0)  //deciding whether or not distance is reasonable
  {
  Serial.println("Out of range");      //if not, print the words "Out of range"
  }
  else
  {
  Serial.print(distance);              //printing the value of the variable "distance"
  Serial.println(" cm");               //printing " cm" after the value
  }
delay(50);

if (Collecter_Index < COLLECTER_SIZE) {
    // while the Collector is not full, keep filling it with numbers
      Collecter[Collecter_Index] = distance;
      Collecter_Index++;
    }
    else
    {// collector is full, continue with other operations

      Serial.println("Run Collector Full");
      //because of the previous counter_Index++ at the end of the if statement, Counter_Index is now at 5
      Collecter_Index--;

      //printf("Collecter_Index: %d", Collecter_Index);
      if (Manager_Index > MANAGER_SIZE-1)
      {// When the Manager array is full, perform comparison (Tom your code) and then start to refill
      
        // TOM'S CODE + TURN THE MOTOR
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
            Serial.println("ERROR NOT GONNA DO SHIT ");
            do_Stuff = 0;
            break;
          }
          else
          {
            Serial.println("MIGHT DO STUFF");
            do_Stuff = 1;
            
          }
        }

        if (do_Stuff != 0)
        {
          if (open_Close == 1)
          {
            Serial.println(" CLOSE THE DAMN CLAW ");
            for(pos = MAX_TURN; pos <=180; pos+=OPEN_CLOSE_SPEED)
            {
              myservo.write(pos);
              delay(15);
            }
            open_Close = 0;
           
          }
          else
          {
            Serial.println("OPEN THE DAMN CLAW ");
            for(pos = 180; pos >=MAX_TURN; pos-=OPEN_CLOSE_SPEED)
            {
              myservo.write(pos);
              delay(15);
            }
            open_Close = 1;
             delay(DELAY);
          }
          
        }


        Serial.println(" Manager Reset");
        for(i = 0; i< MANAGER_SIZE; i++)
        {
          Manager[i]= UPPERLIMIT+ 2; 
        }
        
        Manager_Index = 0; 
      }
      
      // if Manager Array is not full, transfer value from collecter, collecter is full right now

         while (Collecter[Collecter_Index] > UPPERLIMIT && Collecter_Index >= 0)
        {// if the current number (indicated by the index) in the collecter is not within the limit
        //and we can go back to a previous value, do so

          Serial.println(" Run Back Track");
          Collecter_Index--;
        }
        // when the while loops exits, either the current value in the collecter is within limit or  Collecter Index is 0 or both
         // also the collector index either pointing at a value that is within limit or the index is -1 So i have to adjust it

         if (Collecter_Index < 0)
         {
           Collecter_Index = 0;
         }
        
         
        if (Collecter[Collecter_Index] < UPPERLIMIT)
        {// I first check if the current index value is within the range and if it is then I pass it onto Manager, Manger is not
          // full right now

          Manager[Manager_Index] = Collecter[Collecter_Index];
          Serial.println(" RUN FILL Manager");
          Manager_Index++; 
  
        }
        else
        {// now it just means that index is zero and also the value in collecter is not within the limit. Then do nothing

          Serial.println("Run nothing");
        }
    

        
      Serial.println("Collecter Reset");

      Collecter_Index = 0;
      // right now the current Distance reading isn't being stored, so i need to fix that
      Collecter[Collecter_Index] = distance;
      Collecter_Index++;

      
    }
      
    
  }

