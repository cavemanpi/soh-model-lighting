
typedef struct HouseLightSettings {
  int           loops_per_blink;
  unsigned long last_mil_start;
  int           current_loop;
  int           pin;
  bool          blink;
  bool          on;
} HouseLightSettings;

typedef struct StageLightSettings {
  bool on;
} StageLightSettings;

typedef struct ExteriorLightSettings {
  bool on;
} ExteriorLighSetings;

typedef struct GlobalState {
  HouseLightSettings    house_lights;
  StageLightSettings    stage_lights;
  ExteriorLightSettings exterior_lights;
} GlobalState;

GlobalState *global_state;
void setup() {
  // put your setup code here, to run once:
  global_state = (GlobalState*) malloc(sizeof(GlobalState));
  global_state->house_lights.on = true;
  global_state->house_lights.loops_per_blink = 4;
  global_state->house_lights.blink = true;
  global_state->house_lights.current_loop = 0;
  global_state->house_lights.pin = 3;
}

void loop() {
  // put your main code here, to run repeatedly:
  handle_house_lights(&(global_state->house_lights));
}

void handle_house_lights(HouseLightSettings *house_lights) {
  unsigned long now = millis();

  // Note: When I added a pnp transistor to the schematic, the 
  // logic of the analog write flipped on me. So 255 is now off.   
  int pin_on = 55;
  int pin_off = 255;
  
  // The inteded purpose is to have lights inside the opera house model
  // blink prior to playing music kind of like what they do at lobbies of
  // concert halls and such to indicate people should take their seats.
  if (house_lights->blink) {


    
    // If we are ending our last loop, reset counter and turn lights lights will
    // turn on or off appropriately on the next main loop.
    if (house_lights->current_loop >= house_lights->loops_per_blink) {
      house_lights->blink = false;
      house_lights->current_loop = 0;
      house_lights->last_mil_start = 0;
    }
    else {
      unsigned long difference = now - house_lights->last_mil_start;
      
      // Looks like we've been running for around 50 days
      // or an individual blink cycle has completed
      if (now < house_lights->last_mil_start || difference > 2000) {
        house_lights->last_mil_start = now;
        house_lights->current_loop++;
        difference = 0;
      }
      
      // Fade out faster than we fade in, but when we fade back in,
      // remain illumitated for a bit longer than dark (controlled by if condition above)
      long analog_value;
      if(difference < 300) {
        analog_value = 55 + difference;
        if (analog_value > pin_off) {
          analog_value = pin_off;
        }
      }
      else if (difference >= 300 && difference < 500) {
        analog_value = pin_off;
      }
      else {
        analog_value = pin_off - (difference - 500)*.1;
        if (analog_value < pin_on) {
          analog_value = pin_on;
        }
      }
  
      analogWrite(house_lights->pin, analog_value);  
    }
  }
  else {
    // We're not blinking, so should we be on or off?
    if (house_lights->on) {
      analogWrite(house_lights->pin, pin_on);
    }
    else {
      analogWrite(house_lights->pin, pin_off);
    }
  }
}

