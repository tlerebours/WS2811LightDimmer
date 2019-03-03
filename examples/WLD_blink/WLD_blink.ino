#include <WS2811LightDimmer.h>

#define NB_LIGHT 15
#define WS2811_PIN 6
#define WS2811_REFRESH_PERIOD 7

WS2811LightDimmer feux(NB_LIGHT);

void setup() {
  feux.begin(WS2811_PIN);
  for (uint8_t light = 0; light < NB_LIGHT; light++) {
    feux.startBlink(light);
    feux.update();
    delay(60); //decalage de 60 ms entre chaque led
  }
}

void loop() {
  feux.update(WS2811_REFRESH_PERIOD);
}
