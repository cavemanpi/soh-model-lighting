
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

  // The inteded purpose is to have lights inside the opera house model
  // blink prior to playing music kind of like what they do lobbies of
  // concert hall and such to indicate people should take their seats.
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

      long analog_value;
      // Fade out faster than we fade in, but when we fade back in,
      // remain illumitated for a bit longer than dark (controlled by if condition above)
      if(difference < 300) {
        analog_value = 150 - difference/2;
        if (analog_value < 0) {
          analog_value = 0;
        }
      }
      else if (difference >= 300 && difference < 600) {
        analog_value = 0;
      }
      else {
        analog_value = (difference - 600)/5;
        if (analog_value > 150) {
          analog_value = 150;
        }
      }
  
      analogWrite(house_lights->pin, analog_value);  
    }
  }
  else {
    // We're not blinking, so should we be on or off?
    if (house_lights->on) {
      analogWrite(house_lights->pin, 150);
    }
    else {
      analogWrite(house_lights->pin, 0);
    }
  }
}

