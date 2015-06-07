
// Wander - cruise about, turning when an object is detected ahead of us

#define STATE_SELFTEST 0
#define STATE_CRUISING 1
#define STATE_BACKING 2
#define STATE_TURNING 3
#define STATE_LOOK_FOR_DARK 4

//#define DO_WANDER_LOGGING

int state = STATE_SELFTEST;
int selftestPhase = 0;

int turningLeft = 0;
boolean reversingBeepOn;

long clearSteps = -1;
int reversingPiezoPin = PIEZO_PIN;

int darkestHeading;
int brightnessAtDarkestHeading;

void setupWander()
{
  state = STATE_SELFTEST;
  selftestPhase = 0;
  buildPattern();
}


void loopWander()
{

 int range = collisionDetector.getRangeInCm();

  boolean seeObject = range != 0 && range < 10;
  
  switch( state )
  {
    case STATE_SELFTEST:
      doSelftest();
      break;
        
    case STATE_CRUISING:
      if( seeObject )
      {
       // we see a thing! 
       startBacking();
      }
      else
      {
        if (twoWheel.arrived())
          doNextMovement();
      }
      
      break;
   
   case STATE_BACKING:
   
     reversingBeep();
        
    // did we back far enough ?    
    if( twoWheel.arrived() )
    {
       startTurning();
       noTone(reversingPiezoPin); // finish any left-over reversing beep
    }
    break;
    
   case STATE_TURNING:
     if ( ! seeObject)    // nothing there 
     {
       if( twoWheel.arrived()) // turned enough
         startCruising();      // stop turning
       else
         ; // do nothing, still turning to do  
     }
     else
     {
       keepTurning(); // still an obstacle, so we want to keep turning
     }
     break;
     
    case STATE_LOOK_FOR_DARK:
      keepLookingForDark();
      break;
    
  }
  
  
}

void doSelftest()
{
  if( selftestPhase == 0 )
  {
    selftestPhase = 1;
    twoWheel.turn( -10 );
  }
  else if( selftestPhase == 1 )
  {
    if( twoWheel.arrived() )
    {
      selftestPhase = 2;
      tone(reversingPiezoPin, 8000);
      twoWheel.turn( 20 );
    }
  }
  else if( selftestPhase == 2 )
  {
    if( twoWheel.arrived() )
    {
      noTone(reversingPiezoPin);
      selftestPhase = 3;
      twoWheel.turn( -10 );
    }
  }
  else if( selftestPhase == 3 )
  {
    if( twoWheel.arrived() )
      startCruising();
  }
}

void reversingBeep()
{
  // reversing beeps
  // beep for the first 0.5s of each 1.5s period
      long t = millis() % 1000;
      if( t < 200 && t > 0 )
      {
        //if( ! reversingBeepOn )
        {
          tone(reversingPiezoPin, 5000);
          reversingBeepOn = true;
        }
      }
      else
      {
        if( reversingBeepOn )
        {
          noTone(reversingPiezoPin);  
          reversingBeepOn = false;
        }
      }
}

void startCruising()
{
  #ifdef DO_WANDER_LOGGING
   Serial.println( "*****************************************************************************************************" );
   Serial.println( "startCruising " );
#endif

  state = STATE_CRUISING;
  doNextMovement();
  //twoWheel.goForever();
}

void startBacking()
{
#ifdef DO_WANDER_LOGGING
  Serial.println( "*****************************************************************************************************" );
   Serial.println( "startBacking " );
#endif

  state = STATE_BACKING;
  twoWheel.go( -25.0 ); // go back a bit to get away from the obstacle
}

void startTurning()
{
#ifdef DO_WANDER_LOGGING
  Serial.println( "*****************************************************************************************************" );
   Serial.println( "startTurning " );
#endif

  state = STATE_TURNING;  
  turningLeft = random( 0, 2 ); // 0 or 1
  
  keepTurning();

}

void keepTurning()
{
#ifdef DO_WANDER_LOGGING
  Serial.println( "*****************************************************************************************************" );
   Serial.println( "keepTurning " );
#endif

  // Called at the start of a turn and when we find we still have an obstacle during a turn
  int angle = random(10, 90);
  if( turningLeft )
    angle = -angle;
    
  twoWheel.turn( angle );  // turn by a random amount
}

void startLookingForDark()
{
#ifdef DO_WANDER_LOGGING
  Serial.println( "*****************************************************************************************************" );
   Serial.println( "startLookForDark " );
#endif

  state = STATE_LOOK_FOR_DARK;
  
  darkestHeading = twoWheel.getHeading();
  brightnessAtDarkestHeading = collisionDetector.getAmbientBrightness();


  turningLeft = random( 0, 2 ); // 0 or 1
  
  int angle = 360;
  
  if( turningLeft )
    angle = -angle;
    
  twoWheel.turn( angle );  // turn by a random amount

}

void keepLookingForDark()
{
  if( ! twoWheel.arrived() )
  {
    // keep loooking for the darkest direciton
   int brightness = collisionDetector.getAmbientBrightness(); 
 
   if( brightness < brightnessAtDarkestHeading ) // this direction is darker, so remember it
   {
     brightnessAtDarkestHeading = brightness;
     darkestHeading = twoWheel.getHeading();
   }
  }
  else
  {
    //TODO - turn to the darkest direction then start cruising
  }
  
}

