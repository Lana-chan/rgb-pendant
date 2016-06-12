// RGB Pendant by Svetlana 2014-10-10
// Requires updated ATtiny85 hardware files and linker
// https://code.google.com/p/arduino-tiny/
// http://forum.arduino.cc/index.php/topic,116674.0.html

byte purpleFade[] = {255,  0,180,
                       0,  0,  0};

byte rgbFade[] = {255,  0,  0,
                    0,  0,  0,
                    0,255,  0,
                    0,  0,  0,
                    0,  0,255,
                    0,  0,  0,
                  255,255,  0,
                    0,  0,  0,
                  255,  0,255,
                    0,  0,  0,
                    0,255,255,
                    0,  0,  0};

byte oldColor[] = {0, 0, 0};
byte newColor[] = {0, 0, 0};
byte ledPins[] = {0, 1, 4};
byte buttonPin = 2;
boolean buttonLatch = false;
byte mode = 0;
byte stp = 0;
float hue = 0;

void setup() {
  //RGB output pins
  for(byte n = 0; n < 3; n++) pinMode(ledPins[0],OUTPUT);
  //by using internal pullup we can check if the switch
  //  sinks the current to ground, and remove the requirement
  //  for an external resistor
  pinMode(buttonPin,INPUT_PULLUP);
}

void loop() {
  switch(mode) {
    case 0: // rainbow cycle
      hue = fmod(++hue,360);
      hsi2rgb(hue,1,1);
      writeColor();
      break;
    case 1: // purple pulse
      goCycle(purpleFade,sizeof(purpleFade)/3);
      break;
    case 2: // rgb cycle
      goCycle(rgbFade,sizeof(rgbFade)/3);
      break;
    case 3: // rgb blink
      goBlink(rgbFade,sizeof(rgbFade)/3);
    default:
      break;
  }
  
  byte but = digitalRead(buttonPin);
  if(but == LOW && buttonLatch == false) { // mode cycle
    buttonLatch = true;
    mode = (mode+1)%4;
    
    //initialize variables
    for(byte n = 0; n < 3; n++) {
      oldColor[n] = 0;
      newColor[n] = 0;
    }
    hue = 0.01;
    stp = 0;
  } else if(but == HIGH) {
    buttonLatch = false;
  }
  
  delay(20);
}

void goCycle(byte* cycle, int siz) {
  if(hue >= 1) {
    hue -= 1;
    for(byte n = 0; n < 3; n++) {
      oldColor[n] = newColor[n];
      newColor[n] = cycle[stp*3+n];
    }
    stp = (stp+1)%siz;
  }
  mixPins(hue);
  hue += 0.01;
}

void goBlink(byte* cycle, int siz) {
  byte sstp = stp*3;
  newColor[0] = cycle[sstp];
  newColor[1] = cycle[sstp+1];
  newColor[2] = cycle[sstp+2];
  writeColor();
  delay(100);
  newColor[0] = 0; newColor[1] = 0; newColor[2] = 0;
  writeColor();
  delay(300);
  stp = (stp+2)%siz;
}

//output a mix from old to new color by a factor of q (0-1)
void mixPins(float q) {
  for(byte n = 0; n < 3; n++) analogWrite(ledPins[n], byte(oldColor[n]*(1-q)+newColor[n]*q));
}

//output new color
void writeColor() {
  for(byte n = 0; n < 3; n++) analogWrite(ledPins[n], newColor[n]);
}

#define r120 2.09439
#define r240 4.188787
#define r60  1.047196667

//source: http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi-colorspace
void hsi2rgb(float H, float S, float I) {
  byte r, g, b;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = PI*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;
  float It = 255*I/3;
   
  // Math! Thanks in part to Kyle Miller.
  if(H < r120) {
    float Hc = cos(H)/cos(r60-H);
    r = It*(1+S*Hc);
    g = It*(1+S*(1-Hc));
    b = It*(1-S);
  } else if(H < r240) {
    H = H - r120;
    float Hc = cos(H)/cos(r60-H);
    g = It*(1+S*Hc);
    b = It*(1+S*(1-Hc));
    r = It*(1-S);
  } else {
    H = H - r240;
    float Hc = cos(H)/cos(r60-H);    
    b = It*(1+S*Hc);
    r = It*(1+S*(1-Hc));
    g = It*(1-S);
  }
  newColor[0]=r;
  newColor[1]=g;
  newColor[2]=b;
}
