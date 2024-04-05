#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

extern lv_obj_t * ui_arc1;
extern lv_obj_t * ui_arc2;
extern lv_obj_t * ui_Label3;
extern lv_obj_t * ui_Label5;
extern lv_obj_t * ui_Boost;
extern lv_obj_t * ui_Ethanol;
extern lv_obj_t * ui_IntakeTemp;
extern lv_obj_t * ui_Bar1;
extern lv_obj_t * ui_ButtonToSC1;
extern lv_obj_t * ui_ButtonToSc2;
/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

int OilTemp = 0;
int EngTemp = 0;  
float Boost = 0;
int EthanolPercentage = 0;
int IntakeTemp = 0;

int dimPin = 5;      // Input pin for dim signal
int dimState = 0;
int backlightPin = 6; // Output pin for backlight
int brightness = 0;
int ButtonPin = 4; // Input pin for button press for screen switching
int ButtonState = 0;


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register( &indev_drv );


    ui_init();

    Serial.println( "Setup done" );
}

void pressButtonOnActiveScreen() {
  ButtonState = digitalRead(ButtonPin);
   if (ButtonState == HIGH){
    lv_obj_t *active_screen = lv_scr_act();
    if (active_screen == lv_obj_get_parent(ui_ButtonToSc2)) {
        lv_event_send(ui_ButtonToSc2, LV_EVENT_PRESSED, NULL);
    } 
    else if (active_screen == lv_obj_get_parent(ui_ButtonToSc1)) {
        lv_event_send(ui_ButtonToSc1, LV_EVENT_PRESSED, NULL);
    }
    delay(200);
   }
   else{}
}

void loop()
{
  OilTemp = 205;
  EngTemp = 195;
  Boost = 14.5;
  EthanolPercentage = 85;
  IntakeTemp = 65;
  
  lv_arc_set_value(ui_Arc1, OilTemp);
  lv_arc_set_value(ui_Arc2, EngTemp);
  lv_bar_set_value(ui_Bar1, Boost, LV_ANIM_ON);

  char cstr[16];
  itoa(OilTemp, cstr, 10);
  lv_label_set_text(ui_Label3, cstr);

  char cstr2[16];
  itoa(EngTemp, cstr2, 10);
  lv_label_set_text(ui_Label5, cstr2);

  char cstr3[16];
  snprintf(cstr, sizeof(cstr3), "%.1f", Boost);
  lv_label_set_text(ui_Boost, cstr3);

  char cstr4[16];
  itoa(EthanolPercentage, cstr4, 10);
  lv_label_set_text(ui_Ethanol, cstr4);

  char cstr5[16];
  itoa(IntakeTemp, cstr5, 10);
  lv_label_set_text(ui_IntakeTemp, cstr5);
  
  // Read and set brightness based on dimmer input  
  dimState = digitalRead(dimPin);
   if (dimState == HIGH){
    brightness = 50;  // dim backlight
    analogWrite(backlightPin, brightness);
  } 
   else {
    brightness = 250;  // backlight full
    analogWrite(backlightPin, brightness);
  }

  //Screen switching hardware button
  pressButtonOnActiveScreen(); 

    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
