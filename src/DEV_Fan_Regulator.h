struct DEV_Ceiling_Fan : Service::Fan {               // ON/OFF LED

  int relayPin_1speed;
  int relayPin_2speed;
  int relayPin_FullSpeed;
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Ceiling_Fan() : Service::Fan()
  {       // constructor() method

    new Characteristic::Active();             
      new Characteristic::RotationDirection();                        // NEW: This allows control of the Rotation Direction of the Fan
      (new Characteristic::RotationSpeed(50))->setRange(0,100,25);
    
    // power=new Characteristic::On();                 
    // this->relayPin=relayPin;                            
    pinMode(relayPin_FullSpeed,OUTPUT);                         
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,power->getNewVal());      
   
    return(true);                               // return true
  
  } // update
};