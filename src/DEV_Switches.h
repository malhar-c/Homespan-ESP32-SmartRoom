#define man_sw_1 18

bool previous_state;
bool state_change;
bool reboot = 1;  //This variable is to detect if the ESP has restarted or not.

// prototypes
int invert_state(int);
bool Switch_pressed(short);

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

  void loop()
  {
    // Logic for manual switch operation
    // Checking for manual switch state change and whether the device is rebooted
    if(Switch_pressed(man_sw_1) && !reboot)
    {
      delay(50); //debouce delay
      power->setVal(invert_state(digitalRead(man_sw_1))); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }
    reboot = 0;
  }
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

bool Switch_pressed(short man_switch)
{
  if(digitalRead(man_switch) == previous_state){
    state_change = 0;
  }
  else{
    state_change = 1;
  }
  previous_state = digitalRead(man_switch);
  return state_change;
}
