// prototype
int invert_state(int);

struct DEV_Light_pair_1 : Service::LightBulb {               // ON/OFF LED

  int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Light_pair_1(int relayPin) : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    this->relayPin=relayPin;                            
    pinMode(relayPin,OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                       
    
  } // end constructor

  boolean update(){                              // update() method

    digitalWrite(relayPin,invert_state(power->getNewVal())); 
    // power->setVal(1);
    return(true);                               // return true
  
  } // update
};

//Light pair 2 relay control
struct DEV_Light_pair_2 : Service::LightBulb {               // ON/OFF LED

  int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Light_pair_2(int relayPin) : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    this->relayPin=relayPin;                            
    pinMode(relayPin,OUTPUT);  
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                       
    
  } // end constructor

  boolean update(){                              // update() method

    digitalWrite(relayPin,invert_state(power->getNewVal()));      
   
    return(true);                               // return true
  
  } // update
};

//Tube Light relay control
struct DEV_Tube_Light : Service::LightBulb {               // ON/OFF LED

  int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Tube_Light(int relayPin) : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    this->relayPin=relayPin;                            
    pinMode(relayPin,OUTPUT);  
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                      
    
  } // end constructor

  boolean update(){                              // update() method

    digitalWrite(relayPin,invert_state(power->getNewVal()));      
   
    return(true);                               // return true
  
  } // update
};

//Night lamp relay control
struct DEV_Night_Light : Service::LightBulb {               // ON/OFF LED

  int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Night_Light(int relayPin) : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);
    // Serial.println(power->getNewVal());                
    this->relayPin=relayPin;                            
    pinMode(relayPin,OUTPUT);  
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)
                           
    
  } // end constructor

  boolean update(){                              // update() method

    digitalWrite(relayPin,invert_state(power->getNewVal()));      
   
    return(true);                               // return true
  
  } // update
};

//Main power socket relay control
struct DEV_Socket : Service::Switch {               // ON/OFF LED

  int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Socket(int relayPin) : Service::Switch()
  {       // constructor() method

    power=new Characteristic::On(0, true);
    // Characteristic::OutletInUse();                
    this->relayPin=relayPin;                            
    pinMode(relayPin,OUTPUT);   
    // power->setVal(1);
    // digitalWrite(relayPin, LOW); //turn this relay ON by default    
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)            
    
  } // end constructor

  boolean update(){                              // update() method

    digitalWrite(relayPin,invert_state(power->getNewVal()));      
   
    return(true);                               // return true
  
  } // update
};

int invert_state(int in_state)
{
  return(!in_state);
}