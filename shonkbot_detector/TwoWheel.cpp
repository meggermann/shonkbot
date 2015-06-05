#include "TwoWheel.h"



TwoWheel::TwoWheel( AccelStepper *_leftStepper, AccelStepper *_rightStepper, float _stepsPerRev, float _wheelDiameter, float _wheelSpacing )
{
  leftStepper = _leftStepper;
  rightStepper = _rightStepper;
  stepsPerRev = _stepsPerRev;
  wheelDiameter = _wheelDiameter;
  wheelSpacing = _wheelSpacing;
  stepsPerTurn = stepsForTurn( 360.0 );
  heading  = 0.0;
  xPos = 0.0;
  yPos = 0.0;
}

void TwoWheel::setup()
{}

void TwoWheel::loop()
{
  long lastLeftSteps = leftStepper->currentPosition();
  long lastRightSteps = rightStepper->currentPosition();
  
  leftStepper->run();
  rightStepper->run();
  
  long newLeftSteps = leftStepper->currentPosition();
  long newRightSteps = rightStepper->currentPosition();
  
  // Keep track of how far we've gone forwards and clockwise
  
  // Right stepper is positive for forwards, left stepper is negative for forwards
  long forwardSteps = (newRightSteps - lastRightSteps) - (newLeftSteps - lastLeftSteps);
  
  if( forwardSteps != 0 )
  {
      float distance = distanceForSteps( forwardSteps );
      float headingRadians = radians( heading );
      
      xPos += distance * cos( headingRadians );
      yPos += distance * sin( headingRadians );
  }
  
  totalForwardSteps += forwardSteps;
  
  // Both steppers are negative for a clockwise turn
  totalTurnSteps -=  (newRightSteps - lastRightSteps) + (newLeftSteps - lastLeftSteps);
  
  heading = (float)(totalTurnSteps % stepsPerTurn); // keep track of our overall heading
}

void TwoWheel::go( float distance )
{
  float steps = stepsForDistance( distance );
  
  targetSteps = leftStepper->currentPosition() - steps;
  targetIsBigger = steps < 0;
  
  /*
  Serial.println( "*****************************************************************************************************" );
   Serial.print( "go targetSteps " );
   Serial.print( targetSteps );
        
   Serial.print( " targetIsBigger " );
   Serial.println( targetIsBigger );
 */
 
  leftStepper->move(-steps);
  rightStepper->move(steps);
}

void TwoWheel::turn( float degrees ) //  clockwise
{
  float theta = radians(degrees);
  
  float distance = wheelSpacing * 0.5 * theta ;
  float steps = stepsForDistance( distance );
  
  targetSteps = leftStepper->currentPosition() - steps;
  targetIsBigger = steps < 0;

/*
    Serial.println( "*****************************************************************************************************" );
   Serial.print( "turn targetSteps " );
   Serial.print( targetSteps );
        
   Serial.print( " targetIsBigger " );
   Serial.println( targetIsBigger );
   */
     
  leftStepper->move(-steps);
  rightStepper->move(-steps);
}
  
  
boolean TwoWheel::arrived()
{
  long pos = leftStepper->currentPosition();
  
  if( targetIsBigger )
    return pos >= targetSteps-1;
  else
    return pos <= targetSteps+1;
}
  
void TwoWheel::goForever()
{
  //leftStepper->move(-1000);
  //rightStepper->move(1000);
  
  targetSteps = -MAX_STEPS;
  targetIsBigger = false;
  
  #ifdef DEBUG
     Serial.println( "*****************************************************************************************************" );
   Serial.println( "goForever " );
 #endif
 
  leftStepper->move(-MAX_STEPS);
  rightStepper->move(MAX_STEPS);
}
  
void TwoWheel::turnLeftForever()
{
  
#ifdef DEBUG
      Serial.println( "*****************************************************************************************************" );
   Serial.println( "goForever " );
#endif

    targetSteps = MAX_STEPS;
  targetIsBigger = true;

  leftStepper->move(MAX_STEPS);
  rightStepper->move(MAX_STEPS);
}

void TwoWheel::turnRightForever()
{
      
#ifdef DEBUG
   Serial.println( "*****************************************************************************************************" );
   Serial.println( "goForever " );
#endif

    targetSteps = -MAX_STEPS;
    targetIsBigger = false;

  leftStepper->move(-MAX_STEPS);
  rightStepper->move(-MAX_STEPS);

}
  
  

float TwoWheel::stepsForDistance( float distance )
{
  return       distance * 
          ( 2 *                         // because we're half-stepping the motors with HALF4WIRE
          stepsPerRev ) / 
          (3.1415 * wheelDiameter);  // circumference
}


float TwoWheel::distanceForSteps( float steps )
{
  return steps * (3.1415 * wheelDiameter) / (2 * stepsPerRev );                       
          
         
}

float TwoWheel::stepsForTurn( float degrees ) //  clockwise
{
  float theta = radians(degrees);
  
  float distance = wheelSpacing * 0.5 * theta ;
  float steps = stepsForDistance( distance );
  
  return steps;
}  

