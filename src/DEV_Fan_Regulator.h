struct DEV_Ceiling_Fan : Service::Fan {               // ON/OFF LED

  int relayPin_1speed = 25;
  int relayPin_2speed = 33;
  int relayPin_FullSpeed = 32;
  SpanCharacteristic *FanSpeed;                        // reference to the speed Characteristic
  SpanCharacteristic *power;
  
  DEV_Ceiling_Fan() : Service::Fan()
  {       // constructor() method

    power = new Characteristic::Active(0, true);                                   // NEW: This allows control of the Rotation Direction of the Fan
      FanSpeed = (new Characteristic::RotationSpeed(75, true))->setRange(0,100,25);
    
    // power=new Characteristic::On();                 
    // this->relayPin=relayPin;                            
    pinMode(relayPin_FullSpeed,OUTPUT);                         
    pinMode(relayPin_1speed,OUTPUT);
    pinMode(relayPin_2speed,OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)

  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,power->getNewVal());

    if(power->getNewVal())
    {
      switch (FanSpeed->getNewVal())
      {
      case 0: //when speed is 0
        digitalWrite(relayPin_1speed, HIGH);
        digitalWrite(relayPin_2speed, HIGH);
        digitalWrite(relayPin_FullSpeed, HIGH);
        break;
    
      case 25: //when the speed is 25% (Speed position 1)
        digitalWrite(relayPin_1speed, LOW);
        digitalWrite(relayPin_2speed, HIGH);
        digitalWrite(relayPin_FullSpeed, HIGH);
        break;

      case 50: //when the speed is 50% (Speed position 2)
        digitalWrite(relayPin_2speed, LOW);
        digitalWrite(relayPin_1speed, HIGH);
        digitalWrite(relayPin_FullSpeed, HIGH);
        break;

      case 75: //speed is 75% or speed position 3
        digitalWrite(relayPin_1speed, LOW);
        digitalWrite(relayPin_2speed, LOW);
        digitalWrite(relayPin_FullSpeed, HIGH);
        break;

      case 100: //speed is 100% full speed position 4
        digitalWrite(relayPin_FullSpeed, LOW);
        digitalWrite(relayPin_1speed, HIGH);
        digitalWrite(relayPin_2speed, HIGH);
        break;
    
      default:
        break;
      }
    }

    else //turn it off
    {
      digitalWrite(relayPin_1speed, HIGH);
      digitalWrite(relayPin_2speed, HIGH);
      digitalWrite(relayPin_FullSpeed, HIGH);
    }

    
   
    return(true);                               // return true
  
  } // update
};

// int invert_state(int in_state)
// {
//   return(!in_state);
// }