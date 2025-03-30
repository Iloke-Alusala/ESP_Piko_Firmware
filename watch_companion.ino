#include <Arduino.h>
#include "TFT_eSPI.h"
#include "MINGO.h"
#include "Flower_240x280.h"

#define BL 4
#define inLed 2

#define v_R 34

#define b_1 35
#define b_2 33
#define b_3 25
#define b_4 32

// TFT Setup
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library 
TFT_eSprite spritte = TFT_eSprite(&tft); // Sprite object "spritte" created


// Menu Setup
int screenW = 240;
int screenH= 280;
int textWidth;
int x;

bool mainMenu = true ;
bool subMenu = false;
bool wait = false;
//SubMenu
const int menuCount = 4;
bool menuChange = true;
int selectedOption = 0;

const char* menuText[menuCount] = {"Brightness","Time","Setting","About"};
// Time setting menu

// page handler
int page IRAM_ATTR = 0;
bool pageChange = true; // tos top the code changing the page, before the previous page finish loading
bool pageRefresh = true; //Clean the page

//time
volatile int sec IRAM_ATTR= 50;
volatile int minute IRAM_ATTR= 59;
volatile int hrs IRAM_ATTR= 23;
volatile int days IRAM_ATTR= 7;
volatile int months IRAM_ATTR= 4;
volatile int yrs IRAM_ATTR= 2024;

volatile int days_Max IRAM_ATTR;
volatile int daysOfWeekCount IRAM_ATTR;

const char* daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* monthsOfYear[] = {"NULL","January", "February", "March", "April", "May", "June", "July", "August", "September", "Octobor", "November", "December"};

const unsigned long interval = 1000; 
//
// input
int voltage;
bool inp1,inp2,inp3,inp4;


// brightness controll
int mapRange(int input, int input_start, int input_end, int output_start, int output_end) {
    return (input - input_start) * (output_end - output_start) / (input_end - input_start) + output_start;
}
int brightness = 80;
int Level = 4;

// Time Menu controll
int timeMenu_Select = 0;
const int timeMenu = 3;
bool timePage = false;

int clockSelect = 0;
    int dateSelect = 0;
////////////////////////////////////////////////////////////////////
void input(void *pvParameters);
void v_Read(void *pvParameters);
void tft_page(void *pvParameters);
void onTimer(TimerHandle_t xTimer);


void setup() 
{

    Serial.begin(115200);

  pinMode(b_1, INPUT);pinMode(b_2, INPUT);pinMode(b_3, INPUT);pinMode(b_4, INPUT);pinMode(v_R, INPUT); //settup mode
  pinMode(BL, OUTPUT);pinMode(inLed, OUTPUT);
 
  
  
  //Setup TFT
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_ORANGE);

      analogWrite(BL, 0);
        //    for (int i = 0; i < 3; i++) {
        //     digitalWrite(inLed, HIGH);  // Turn on the LED
        //     delay(500);                  // Wait for 500 millisec (0.5 sec)
        //     digitalWrite(inLed, LOW);   // Turn off the LED
        //     delay(500);                  // Wait for another 500 millisec
        // }
      digitalWrite(inLed, HIGH);
      analogWrite(BL, brightness);

  xTaskCreatePinnedToCore(input,"button", 1024, NULL, 1, NULL, 1); // core 1
  xTaskCreatePinnedToCore(v_Read,"Voltage Read",1024, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(tft_page,"Page of TFT",20000, NULL, 2, NULL, 1);

  TimerHandle_t timerHandle = xTimerCreate("timer", pdMS_TO_TICKS(interval), pdTRUE, 0, onTimer); // name, period, auto reload, timer id, callback
  if (timerHandle != NULL) {
    xTimerStart(timerHandle, 0);
  }

}

void loop() {
  
 
    //Serial.printf("%02d:%02d:%02d\n", hrs, minute, sec);

    //Serial.print("months : ");
    //Serial.println(monthsOfYear[months]);
    //Serial.print("day : ");
    //Serial.println(daysOfWeek[days]);

    //button read//
   // if (inp1 == HIGH ){
    //  Serial.print("35");
    //}
    //page read//
    //Serial.print("PG: ");
    //Serial.println(page);
    //Serial.print("change: ");
    //Serial.println(pageChange);
    //Serial.print("Refresh: ");
    //Serial.print(pageRefresh);
      delay(1000);

}

void input (void *pvParameters){
  
  while(1){
  inp1=digitalRead(b_1);
  inp2=digitalRead(b_2);
  inp3=digitalRead(b_3);
  inp4=digitalRead(b_4);

  if(inp1==HIGH && pageChange ==true)
  { 
    pageChange=false;
    pageRefresh=true;
    page++; //NEXT page
  }
  
  if(inp2==HIGH && pageChange ==true)
  {
    pageChange=false;
    pageRefresh=true;
    page--; // Prev Page
  }
  
  if(page ==1 && mainMenu == true&& inp3==HIGH && pageChange ==true && wait==false) //menu select controll
  {
    pageRefresh=true;
     selectedOption = (selectedOption + 1) % menuCount;
     Serial.println(selectedOption);
      Serial.println("main");
  }
  
  if(inp4==HIGH && pageChange ==true && mainMenu == true && page == 1 && wait== false)  //menu enter control
  { 
    wait =  true;
    pageChange = false;
    mainMenu = false;
    subMenu= true;
    pageRefresh=true;

    Serial.print("INP4");

  }

  if(selectedOption == 1 && inp3==HIGH && subMenu==true && mainMenu==false&& wait==false)  //time setting menu controll
  { 
    
    timePage =false;
    pageRefresh=true;
    //Serial.print("press clock menu");
    timeMenu_Select = (timeMenu_Select + 1) % timeMenu ;
     //Serial.println(timeMenu_Select);
  }

  //page number handler
  page>3?page=0:page<0?page=3:page=page;
  vTaskDelay(200);
  }
}


void tft_page(void *pvParameters)
{
  while(1)
  {
    if(page==0 && mainMenu == true) // main
    {
        if(pageRefresh==true){
          tft.fillScreen(TFT_BLACK);
          pageRefresh= false;
          Serial.print("refresh");
        }
        if(pageRefresh==false){
          //Page
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            // years // 
              tft.setTextSize(3);
              tft.setCursor(85, 73); //xy
              tft.printf("%04d\n", yrs);
            // date/month //
              tft.setTextSize(3);
              tft.setCursor(75,101); //xy
              tft.printf("%02d/%02d\n", days, months);
            // time
              tft.setTextSize(4);
              tft.setCursor(28, 130); //xy
              tft.printf("%02d:%02d:%02d\n", hrs, minute, sec);
              tft.setTextSize(3);
            // day
              textWidth = tft.textWidth(daysOfWeek[daysOfWeekCount]);
              x = (screenW - textWidth) / 2;
              tft.setCursor(x, 165); //xy
              tft.print(daysOfWeek[daysOfWeekCount]);
              textWidth = tft.textWidth(monthsOfYear[months]);
            // months
              x = (screenW - textWidth) / 2;
              tft.setCursor(x, 193); //xy
              tft.print(monthsOfYear[months]);
            vTaskDelay(100);
            pageChange=true;
        }  
    }   
    else if(page==1 && mainMenu == true && wait==false) //menu
    { wait ==true;
        if(pageRefresh==true){
          tft.fillScreen(TFT_BLACK);
          pageRefresh= false;
        }
        if(pageRefresh==false){
          //isi
             tft.setTextSize(2);
                const int menuItemWidth = 200;  // Width of each menu item
                const int menuItemHeight = 50;  // Height of each menu item
                const int menuItemSpacing = 10; // Spacing between menu items

                int totalMenuHeight = menuCount * (menuItemHeight + menuItemSpacing) - menuItemSpacing;
                int menuStartY = (tft.height() - totalMenuHeight) / 2;

                for (int i = 0; i < menuCount; i++) {
                  
                  int x = (tft.width() - menuItemWidth) / 2;
                  int y = menuStartY + i * (menuItemHeight + menuItemSpacing);

                  int xText = (screenW - tft.textWidth(menuText[i]))/ 2; 
                  

                  if (i == selectedOption) {
                    tft.fillRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_YELLOW);
                    tft.setTextColor(TFT_BLACK);
                  } else {
                    tft.drawRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_WHITE);
                    tft.setTextColor(TFT_WHITE);
                  }

                  tft.drawString(String(menuText[i]) , xText, y + 15);
                     // Serial.println("wifiSub");
                }
            
            pageChange=true;
          
        }  
        vTaskDelay(100);
      wait== false;
    }
    else if(page==2 && mainMenu == true) //flower
    { 
        if(pageRefresh==true){
          tft.fillScreen(TFT_BLACK);
          pageRefresh= false;
        }
        if(pageRefresh==false){

          // isi
            tft.pushImage(0,0,screenW,screenH,Flower_240x280);
          vTaskDelay(100);
          pageChange=true; 
        }  

    }
    else if(page==3 && mainMenu == true) // colour
    { 
        if(pageRefresh==true){
          tft.fillScreen(TFT_BLACK);
          pageRefresh= false;
        }
        if(pageRefresh==false){

          // isi
            tft.fillScreen(TFT_PINK);
          vTaskDelay(100);
          pageChange=true; 
        }  

    }

    //Sub menu
    if(mainMenu == false && subMenu ==true &&selectedOption == 0  ){ // brightness control
        String text = "Brightness";
        if(pageRefresh==true){
          tft.fillScreen(TFT_BLACK);
          pageRefresh= false;
        }
        if(pageRefresh==false){
          // isi
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(3);
            x = (screenW - tft.textWidth(text)) / 2;
            tft.setCursor(x,100);
            tft.print(text);
            tft.setTextSize(3);
            x = screenW/2;
            tft.setCursor(x,180);
            tft.print(Level);
              if(inp1 == HIGH){  //increased brightness
                  if(Level < 6){
                  Level++; 
                  brightness = mapRange(Level, 1, 6, 20 ,255);
                  analogWrite(BL, brightness);
                  vTaskDelay(100);}}
                  
                  
              if(inp2 == HIGH){   //decreased brightness
                if(Level > 1){
                Level--;
                brightness = mapRange(Level, 1, 6, 20 ,255);
                analogWrite(BL, brightness);
                vTaskDelay(100);}}
                vTaskDelay(100);

              if(inp4 == HIGH && wait==false){ //return to main
                Serial.print("Ouch");
             mainMenu=true;
             subMenu=false;
             pageChange=true;
             page=1;
             pageRefresh=true;
             vTaskDelay(100);
              }
              wait=false;
           
        }  
        
         
    }
    if(selectedOption == 1 && mainMenu == false && subMenu ==true ){ // Time Control
  
            if(pageRefresh==true){
                  tft.fillScreen(TFT_BLACK);
              
                  pageRefresh= false;
                }
                if(pageRefresh==false){
                  timePage ==false;
                  // isi
                    const int timeMenuWidth = 180;  // Width of each menu item
                    const int timeMenuHeight = 60;  // Height of each menu item
                    const int timeMenuSpacing = 10; // Spacing between menu items
                    
                    
                    String TimeMenuList[timeMenu]={"Clock","Date","Back"};

                    int totalMenuHeight = timeMenu * (timeMenuHeight +  timeMenuSpacing) - timeMenuSpacing;
                        int menuStartY = (tft.height() - totalMenuHeight) / 2;

                        for (int i = 0; i < timeMenu; i++) {
                          int x = (tft.width() - timeMenuWidth) / 2;
                          int y = menuStartY + i * (timeMenuHeight +timeMenuSpacing);

                          if (i == timeMenu_Select) {
                            tft.fillRoundRect(x, y, timeMenuWidth, timeMenuHeight, 10, TFT_YELLOW);
                            tft.setTextColor(TFT_BLACK);
                          } else {
                            tft.drawRoundRect(x, y, timeMenuWidth, timeMenuHeight, 10, TFT_WHITE);
                            tft.setTextColor(TFT_WHITE);
                          }

                          int xText = (screenW - tft.textWidth(TimeMenuList[i]))/ 2; 
                          tft.drawString(String(TimeMenuList[i]) , xText, y + 15);
                            // Serial.println("wifiSub");
                        }
                    vTaskDelay(100);
                    if(inp4==HIGH && wait==false && timeMenu_Select==0){ // to time
                      wait=true;
                      subMenu=false;
                      pageRefresh=true;
                      timePage=true;
                      Serial.println("to time");
                    }
                    if(inp4==HIGH && wait==false && timeMenu_Select==1){ // to date
                      wait=true;
                      subMenu=false;
                      pageRefresh=true;
                      timePage=true;
                      Serial.println("to date");
                    }
                    if(inp4 == HIGH && wait==false && timeMenu_Select==2 ){ //return to main
                        Serial.print("Ouch");
                    mainMenu=true;
                    subMenu=false;
                    pageChange=true;
                    page=1;
                    pageRefresh=true;
                    vTaskDelay(100);
                      }
                    wait=false;
                    
                } 
            }

    if(selectedOption == 2 && mainMenu == false && subMenu ==true ){ //setting
      
        if(pageRefresh==true){
              tft.fillScreen(TFT_BLACK);
              pageRefresh= false;
            }
            if(pageRefresh==false){
              // isi
                tft.pushImage(0,0,screenW,screenH,MINGO);
                vTaskDelay(100);
                if(inp4 == HIGH && wait==false){ //return to main
                Serial.print("Ouch");
             mainMenu=true;
             subMenu=false;
             pageChange=true;
             page=1;
             pageRefresh=true;
             vTaskDelay(100);
              }
              wait=false;
              
            }  
    }
    if(selectedOption == 3 && mainMenu == false && subMenu ==true){ //about the esp32

          if(pageRefresh==true){
                tft.fillScreen(TFT_BLACK);
                pageRefresh= false;
              }
              if(pageRefresh==false){
                // isi
                const char *message[] = {"ESP32-WROOM","ESP32-D0WDQ6","Chip-v4.4.3","ID:DCC84E9EF0C8","Speed-240 Mhz","Flash:4.19 MB","F-Speed:80 Mhz","Flash Mode:0","F-Used:674.06 KB","Cores:2","RAM:256 KB"};
                menuChange=false;
                tft.setTextSize(2);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                textWidth = tft.textWidth(message[0]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 27); //xy
                tft.print(message[0]);

                textWidth = tft.textWidth(message[1]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 45); //xy
                tft.print(message[1]);

                textWidth = tft.textWidth(message[2]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 62); //xy
                tft.print(message[2]);

                textWidth = tft.textWidth(message[3]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 79); //xy
                tft.print(message[3]);

                textWidth = tft.textWidth(message[4]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 95); //xy
                tft.print(message[4]);

                textWidth = tft.textWidth(message[5]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 112); //xy
                tft.print(message[5]);

                textWidth = tft.textWidth(message[6]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 128); //xy
                tft.print(message[6]);

                textWidth = tft.textWidth(message[7]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 145); //xy
                tft.print(message[7]);

                textWidth = tft.textWidth(message[8]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 163); //xy
                tft.print(message[8]);

                textWidth = tft.textWidth(message[9]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 180); //xy
                tft.print(message[9]);
                
                textWidth = tft.textWidth(message[10]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 198); //xy
                tft.print(message[10]);

                  vTaskDelay(100);
                  if(inp4 == HIGH && wait==false){ //return to main
                Serial.print("Ouch");
             mainMenu=true;
             subMenu=false;
             pageChange=true;
             page=1;
             pageRefresh=true;
             vTaskDelay(100);
              }
              wait=false;
                
              }  
    }

          //clock setting page
          if(timeMenu_Select==0 && mainMenu == false && timePage ==true ){ //setting

            if(pageRefresh==true){
                  tft.fillScreen(TFT_BLACK);
                  pageRefresh= false;
                }
                if(pageRefresh==false){
                 
                  // isi
                  int menuColumnCount = 3;
                  int menuRowCount = 1;

                  const int menuItemSize = 60; // Smaller size for the menu items
                  const int menuItemSpacing = 10; // Space between menu items
                  const int menuItemCornerRadius = 5; // Rounded corner radius
                  const int highlightBorderWidth = 3; // Width of the highlight border

                  int clockMenuCount = menuColumnCount*menuRowCount;
                  

                    int clockNOW[] = {hrs,minute,sec};
                  
                      int totalMenuWidth = menuColumnCount * (menuItemSize + menuItemSpacing) - menuItemSpacing;
                      int totalMenuHeight = menuRowCount * (menuItemSize + menuItemSpacing) - menuItemSpacing;

                      int menuStartX = screenW/2 - totalMenuWidth / 2;
                      int menuStartY = screenH/2 - totalMenuHeight / 2;

                      for (int i = 0; i < clockMenuCount; i++) {
                        int row = i / menuColumnCount;
                        int col = i % menuColumnCount;

                        int x = menuStartX + col * (menuItemSize + menuItemSpacing);
                        int y = menuStartY + row * (menuItemSize + menuItemSpacing);

                        bool isSelected = (i == clockSelect);
                              tft.setTextSize(2);
                        if (isSelected) {
                          // Draw a highlighted rounded rectangle outline around the menu item
                          tft.drawRoundRect(x - highlightBorderWidth, y - highlightBorderWidth, menuItemSize + 2 * highlightBorderWidth, menuItemSize + 2 * highlightBorderWidth, menuItemCornerRadius + highlightBorderWidth, TFT_ORANGE);
                          tft.setTextColor(TFT_ORANGE,TFT_BLACK);
                          
                        } else {
                          tft.drawRoundRect(x, y, menuItemSize, menuItemSize, menuItemCornerRadius, TFT_WHITE);
                          tft.setTextColor(TFT_WHITE,TFT_BLACK);
                        }

                    // Draw menu item logos and text
                      tft.drawString(String(clockNOW[i]), x + 15, y + menuItemSize / 2 - 5);
                          }
                    vTaskDelay(200);
                    
                    if(inp1==HIGH){
                        Serial.println("HIGH 1");
                        if(clockSelect == 0){
                            hrs++;
                            if(hrs>23){
                              hrs=0;
                            }
                        }
                        if(clockSelect == 1){
                          minute++;
                          if(minute > 59){
                            minute=0;
                          }
                        }
                        if(clockSelect == 2){
                          sec=59;
                        
                        }
                    }
                    if(inp2==HIGH){
                        Serial.println("HIGH 2");
                        if(clockSelect == 0){
                            hrs--;
                            if(hrs<0){
                              hrs=23;
                            }
                        }
                        if(clockSelect == 1){
                          minute--;
                            if(minute < 0){
                                minute=59;
                            }
                        }
                        if(clockSelect == 2){
                          sec = 0;
                        }
                    }
                    if(inp3==HIGH && wait==false){
                                pageRefresh=true;
                                Serial.print("press clock select");
                                clockSelect = (clockSelect + 1) % 3 ;
                                Serial.println(clockSelect);
                                vTaskDelay(100);
                    }
                    if(inp4 == HIGH && wait==false ){ //return to main
                        Serial.print("Ouch");
                    mainMenu=true;
                    subMenu=false;
                    timePage=false;
                    pageChange=true;
                    page=1;
                    pageRefresh=true;
                    vTaskDelay(100);
                      }
             
                  wait=false;
                  
                }  
        }

        if(timeMenu_Select==1 && mainMenu == false && timePage ==true ){ //setting
          
            if(pageRefresh==true){
                  tft.fillScreen(TFT_BLACK);
                  pageRefresh= false;
                }
                if(pageRefresh==false){
                   tft.setTextSize(3);
                const int menuItemWidth = 200;  // Width of each menu item
                const int menuItemHeight = 50;  // Height of each menu item
                const int menuItemSpacing = 10; // Spacing between menu items

            

                const int dateOption = 3;
                int date_num[]={days,months,yrs};

                int totalMenuHeight = dateOption * (menuItemHeight + menuItemSpacing) - menuItemSpacing;
                int menuStartY = (tft.height() - totalMenuHeight) / 2;

                for (int i = 0; i < dateOption; i++) {
                  
                  int x = (tft.width() - menuItemWidth) / 2;
                  int y = menuStartY + i * (menuItemHeight + menuItemSpacing);

                  
                  

                  if (i == dateSelect) {
                    tft.fillRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_YELLOW);
                    tft.setTextColor(TFT_BLACK);
                  } else {
                    tft.drawRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_WHITE);
                    tft.setTextColor(TFT_WHITE);
                  }

                  tft.drawString(String(date_num[i]) , x+10, y + 15);
                     // Serial.println("wifiSub");
                }
                    vTaskDelay(200);
                    
                  wait=false;
                  if(inp1 ==HIGH&&wait==false){
                    if(dateSelect==0){

                      days++;
                      if(days>days_Max){
                        days=1;
                      }
                    }
                    if(dateSelect==1){
                      months++;
                      if(months>12){
                        months=1;
                      }
                    }
                    if(dateSelect==2){
                      yrs++;
                      
                    }
                  }
                  if(inp2 ==HIGH&&wait==false) {
                    if(dateSelect==0){
                      days--;
                      if(days < 1){
                        days=days_Max;
                      }
                    }
                    if(dateSelect==1){
                      months--;
                      if(months<1){
                        months=12;
                      }
                    }
                    if(dateSelect==2){
                      yrs--;
                 
                      if (yrs <0){

                        yrs=9999;
                      }
                    }
                  }
                  if(inp3 ==HIGH &&wait==false){
                       pageRefresh=true;
                                Serial.print("press date select");
                                dateSelect = (dateSelect + 1) % 3 ;
                                Serial.println(dateSelect);
                                vTaskDelay(100);
                  }
                  if(inp4 ==HIGH && wait==false){
                    Serial.print("Ouch");
                    mainMenu=true;
                    subMenu=false;
                    timePage=false;
                    pageChange=true;
                    page=1;
                    pageRefresh=true;
                    vTaskDelay(100);
                  }
                  
                }  
        }
 
  }
}

void v_Read(void *pvParameters) {

while(1)
{
  uint32_t Vbatt = 0;
  for(int i = 0; i < 16; i++) {
    Vbatt = Vbatt + analogReadMilliVolts(v_R); // ADC with correction   
  }
  float Vbattf = 2 * Vbatt / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
  Serial.println(Vbattf, 3);
  delay(1000);
}


}
void onTimer(TimerHandle_t xTimer) {
    sec++;
    
    if (sec > 59) {
        sec = 0;
        minute++;
        if (minute > 59) {
            minute = 0;
            hrs++;
            if (hrs > 23) {
                hrs = 0;
                days++;
                pageRefresh = true;
                if (months == 2) {
                        if (yrs % 4 == 0) {
                            days_Max = 29;
                        } else {
                            days_Max = 28;
                        }
                    } else if (months == 4 || months == 6 || months == 9 || months == 11) {
                        days_Max = 30;
                      } else {
                        days_Max = 31;
                      }
                if (days > days_Max) {
                    days = 1;
                    months++;
                    if (months > 12) {
                        months = 1;
                        yrs++;
                    }
                    
                }
               
            }
        }
    }
    daysOfWeekCount = (days + (13 * (months + 1)) / 5 + yrs % 100 + (yrs % 100) / 4 + (yrs / 100) / 4 + 5 * (yrs / 100)) % 7;
    daysOfWeekCount = (daysOfWeekCount + 6) % 7;
  
}
