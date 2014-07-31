#include "HT1632_GCScoreboard.h"

//PIN DEFINTIONS
#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6

//VARIABLE CONSTANT DEFINITIONS
#define ON   1
#define OFF  0
#define TOP  0
#define BOTTOM 1
#define TOPCHAR 60    // ASCII < (displayed sideways)
#define BOTTOMCHAR 62 // ASCII > (displayed sideways)
#define HOME 1
#define VISITOR 0

// Initialization three matrices!
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3);

//GLOBAL VARIABLE DECLARATION AND INITIALIZATION
  uint8_t BaseOne=OFF;
  uint8_t BaseTwo=OFF;
  uint8_t BaseThree=OFF;
  uint8_t Balls=0;
  uint8_t Strikes=0;
  uint8_t Outs=0;
  uint8_t VScore=0;
  uint8_t HScore=0;
  uint8_t Inning=1;
  uint8_t Half=TOP;
  char inByte; 
  String command="";
  int8_t My_Side=-1;
  unsigned char TeamLogo[]  = 
      {0x7F, 0x40, 0x7F, 0x4C, 0x92, 0x64};//default Logo - US


/////////////////////////////////////////////////////////
//
//                    USAGE STATEMENT
//
/////////////////////////////////////////////////////////

void usage(){
  
    //NOTE:  Had some issues with all of the help statement printing that seemed
    //  related to the length of the output.  Elected to just comment out the
    //  expanded help comments but left them in here for the benefit of 
    //  potential future users.
    
    Serial.println("Valid options for this program are:");
    Serial.println("  SGS VISITOR SCORE, HOME SCORE, TOP/BOTTOM, INNING,");
    Serial.println("    FIRST, SECOND, THIRD,BALLS, STRIKES, OUTS");
//    Serial.println("    where:");
//    Serial.println("        FIRST   - Runner on (1) or not on (0) first base"); 
//    Serial.println("        SECOND  - Runner on (1) or not on (0) second base"); 
//    Serial.println("        THIRD   - Runner on (1) or not on (0) third base"); 
//    Serial.println("        BALLS   - Number of balls (0-3)"); 
//    Serial.println("        STRIKES - Number of strikes (0-2)"); 
//    Serial.println("        OUTS    - Number of outs (0-2)");
    Serial.println("  inning TOP/BOTTOM, INNING");
//    Serial.println("    where:");
//    Serial.println("        TOP/BOTTOM - Which half of the inning we're in");
//    Serial.println("                     (0 for bottom, 1 for top)");
//    Serial.println("        INNING     - Number of inning game is in");
    Serial.flush();
    Serial.println("  score VISITOR_SCORE, HOME_SCORE");
//    Serial.println("    where:");
//    Serial.println("        VISITOR_SCORE - Score of visiting team");
//    Serial.println("        HOME_SCORE    - Score of home team");
    Serial.println("  cheer - Cheer for team");
    Serial.println("  message MESSAGE");
//    Serial.println("    where:");
//    Serial.println("        MESSAGE - message to scroll across screen");
    Serial.println("  setMySide HOME|AWAY");
//    Serial.println("  set your team as either the Home or Away Team");
    Serial.println("  setMyLogo INT,INT,INT,INT,INT,INT");
//    Serial.println("  set your 6x8 team logo bitmap");
//    Serial.println("    where:");
//    Serial.println("        INT - Series of chars (0-255), encoding the ");
//    Serial.println("              logo, each INT representing a column of");
//    Serial.println("              logo, from left to right, LSB on top");
Serial.println("  printScoreUpdate");
//    Serial.println("  print the Score Update Screen prior to scores:)
Serial.println("  MLBRecap VIS, VSCORE, HOME, HSCORE, INNING, T|B");
//    Serial.println("    Print recap of baseball game score to the center screen");
//    Serial.println("    where:");
//    Serial.println("        VIS - Three letter visiting team abbreviation");
//    Serial.println("        VSCORE - Visitor score");
//    Serial.println("        HOM - Three letter home team abbreviation");
//    Serial.println("        HSCORE - Home score");
//    Serial.println("        INNING - Inning number");
//    Serial.println("        T|B - Specifies top(T) or bottom (B) of inning");
Serial.println("  help - This message");
}    




/////////////////////////////////////////////////////////
//
//               MANDATORY SUBROUTINES
//
/////////////////////////////////////////////////////////

//runs on first boot
void setup() {
  
  Serial.begin(9600);
  matrix.begin(HT1632_COMMON_16NMOS);  
  matrix.fillScreen();
  while (! Serial);
  Serial.println("Initialization complete");
  delay(500);
  //draw initial game graphics
  drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
  //Once done, this line must be output so that interfacing programs know the line is clear
  //  for the next command
  Serial.println("Enter command:");

}

//program stays in this loop after setup
void loop(){
  
  while (Serial.available() > 0){
    //read in first word (the command)
    inByte = Serial.read();
    //if Byte is alpha-numeric, add it to command
    if ((inByte >= 65 && inByte <= 90) || (inByte >=97 && inByte <=122) || (inByte >= 48 &&     inByte <=57) || inByte == 43 || inByte == 61 || inByte == 63) {
      command.concat(inByte);
    }
    //if byte is a space or return, process command
    if (inByte == 32 || inByte == 10){
        //Decision tree for accepted commands
         if (command == "cheer"){
           cheer();
         }
         else if (command == "message"){
           print_message();
         }
         else if (command == "inning"){
           set_inning();
         }
         else if (command == "score"){
           set_score();
         }
         else if (command == "SGS"){
           getGameState();
         }  
         else if (command == "test"){
           test();
           drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore,HScore,Half,Inning);
         }
         else if (command == "help"){
           usage();
         }
         else if (command == "setMySide"){
           setMySide();
           }
         else if (command == "setMyLogo"){
           setMyLogo();
         }
         else if (command == "printScoreUpdate"){
           printScoreScreen();
         }
         else if (command == "MLBRecap"){
           MLBRecap();
         }

         //Once done, this line must be output so that interfacing programs know the line is clear
         //  for the next command
         Serial.println("Enter command:");
         Serial.flush();
         
         command="";

    }
  }
}

//subroutine to clear out remainder of serial input buffer
void clearSerial(){
  while (Serial.available() > 0){
      inByte = Serial.read();
      //Serial.println(inByte); 
    }
}

/////////////////////////////////////////////////////////
//
//  Subroutines used to set game elements
//
/////////////////////////////////////////////////////////

//set inning and half
void set_inning(){

    Half = Serial.parseInt(); 
    Inning = Serial.parseInt(); 
    
    if (Serial.read() == '\n') {
      Serial.println("Command received, setting Inning to:");
      if (Half == TOP){
        Serial.print("    Top of Inning ");
      }
      else {
        Serial.print("    Bottom of Inning ");
      }
      Serial.println(Inning);
      drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
      Serial.println("Inning set.");
    }
      drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
}

//set scores
void set_score(){

    VScore = Serial.parseInt(); 
    HScore = Serial.parseInt(); 
    
    if (Serial.read() == '\n') {
      Serial.println("Command received, setting Score to:");
      Serial.print("    Visitor: ");
      Serial.println(VScore);
      Serial.print("    Home   : ");
      Serial.println(HScore);
      drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
      Serial.println("Score set.");
    }
      drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
}

//set runners on base and ball/strike/out count    
void getGameState(){
    
    VScore = Serial.parseInt();
    HScore = Serial.parseInt(); 
    Half = Serial.parseInt(); 
    Inning = Serial.parseInt(); 
    BaseOne = Serial.parseInt(); 
    BaseTwo = Serial.parseInt(); 
    BaseThree = Serial.parseInt(); 
    Balls = Serial.parseInt(); 
    Strikes = Serial.parseInt(); 
    Outs = Serial.parseInt(); 
    
    if (Serial.read() == '\n') {
      Serial.println("Command received, setting Game State to:");
      Serial.print("    Visitor: ");
      Serial.println(VScore);
      Serial.print("    Home   : ");
      Serial.println(HScore);
      Serial.println("---------------------------");
      Serial.print("  Inning :");
      if (Half == TOP){
        Serial.print("Top of ");
      }
      else {
        Serial.print("Bottom of ");
      }
      Serial.println(Inning);
      Serial.println("---------------------------");
      Serial.print("    First Base: ");
      Serial.println(BaseOne);
      Serial.print("   Second Base: ");
      Serial.println(BaseTwo);
      Serial.print("    Third Base: ");
      Serial.println(BaseThree);
      Serial.println("---------------------------");
      Serial.print("         Balls: ");
      Serial.println(Balls);
      Serial.print("       Strikes: ");
      Serial.println(Strikes);
      Serial.print("          Outs: ");
      Serial.println(Outs);
      drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
      Serial.println("Game state set.  ");
    }
 
}

//subroutine to set which side the team logo appears on
void setMySide(){
  
    while (inByte != 'H' && inByte != 'A'){  
    
        inByte = Serial.read();
        //Serial.println(inByte);
    
        if (inByte == 'H'){
          My_Side=HOME; 
          Serial.println("Your team is designated the Home Team");        
        }
        else if (inByte =='A'){
          My_Side=VISITOR;
          Serial.println("Your team is designated the Away Team");        
        }
    }   
    drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore,HScore,Half,Inning);
           
    clearSerial();
  
}



/////////////////////////////////////////////////////////
//
//  These subroutines are the toplevel for drawing
//    game objects (like the diamond, score, outs, etc)
//
/////////////////////////////////////////////////////////

//top-level subroutine that refreshes all game display elements

void drawGameState(uint8_t First, uint8_t Second, uint8_t Third, uint8_t BallCt, uint8_t StrikeCt, uint8_t OutCt,uint8_t V_Score, uint8_t H_Score, uint8_t Inn_Half, uint8_t InnNum){
   matrix.clearScreen();
   drawBaseballDiamond();
   runner(1,First);
   runner(2,Second);
   runner(3,Third);
   balls(BallCt);
   strikes(StrikeCt);
   outs(OutCt);
   score(V_Score,H_Score);
   inning(Inn_Half, InnNum);
   drawHomeAway();
   matrix.writeScreen();
}

//draw home and away labels
void drawHomeAway(){
    if (My_Side == VISITOR){
        matrix.drawLogo(0,8,(uint8_t *) TeamLogo,6,8,1);
    }
    else {  
      matrix.drawSideChar(0,10,86,1,1);
    }
    if (My_Side == HOME){
        matrix.drawLogo(48,0,(uint8_t *) TeamLogo,6,8,1);
        //matrix.drawSideSkinnyChar(48,4,69,1,1);
        //matrix.drawSideSkinnyChar(49,0,66,1,1);
    }
    else {
        matrix.drawSideChar(48,0,72,1,1);
    }
}

//draw the overall baseball diamond
void drawBaseballDiamond(){
   drawDiamond(39,0,8);
}

//draw the bases either with or without runner
void runner(uint8_t base, uint8_t state){

  uint8_t x_top;
  uint8_t y_top;
 
  if ( base == 1){
   x_top=24+20;
   y_top=5;
 }
 else if (base==2){
   x_top=24+15;
   y_top=0;
 }
 else if (base==3){
   x_top=24+10;
   y_top=5;
 }  
 else {
  return;
 }
 
 if (state==ON){
       fillDiamond(x_top,y_top,3);  
 }
 else if (state==OFF){
   drawDiamond(x_top,y_top,3);
 }
 else {
   Serial.print ("ERROR: Invalid value for runner on base ");
   Serial.print(base);
   Serial.println(".  Valid values are 0(OFF) or 1(ON)");
   drawDiamond(x_top,y_top,matrix.height()/8);
 }
}

//draw correct number of balls
void balls(uint8_t num){

    if ( num < 4){
      drawBSO(24,0,num);  
    }
    else {
      Serial.println("ERROR: BALL VALUE MUST BE BETWEEN 0 and 3.");
    }
}

//draw correct number of strikes
void strikes(uint8_t num){

    if ( num < 3){
      drawBSO(24,5,num);  
    }
    else {
      Serial.println("ERROR: STRIKE VALUE MUST BE BETWEEN 0 and 2.");
    }

}

//draw correct number of outs
void outs(uint8_t num){

    if ( num < 3){
      drawBSO(24,13,num); 
    }
    else {
      Serial.println("ERROR: OUT VALUE MUST BE BETWEEN 0 and 2.");
    }

}

//draw score elements
void score(uint8_t HS, uint8_t VS){
  if (VS < 10){
      matrix.drawSideChar(57,1,48+VS,1,2);
  }
  else {
      matrix.drawSideSkinnyChar(56,8,48+(VS/10),1,2);
      matrix.drawSideSkinnyChar(56,0,48+(VS%10),1,2);    
  }

  if (HS < 10){
      matrix.drawSideChar(9,1,48+HS,1,2);
  }
  else {
      matrix.drawSideSkinnyChar(8,8,48+(HS/10),1,2);
      matrix.drawSideSkinnyChar(8,0,48+(HS%10),1,2);    
  }

}

//draw inning elements
void inning(uint8_t TB, uint8_t Inn){
  if (Inn < 10){
    matrix.drawSideChar(48,10,48 + Inn,1,1);
  }
  else {
    matrix.drawSideSkinnyChar(48,12,48+(Inn/10),1,1);
    matrix.drawSideSkinnyChar(48,8,48+(Inn%10),1,1);
  }
  if (TB == TOP){
    matrix.drawChar(0,0,TOPCHAR,1,1);
  }
  else {
    matrix.drawChar(0,0,BOTTOMCHAR,1,1);
  }
}

//Reprograms the team logo from default to user defined
void setMyLogo(){
    for (int i=0; i<6; i++){
        TeamLogo[i]=Serial.parseInt();
    }
    drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
 
    clearSerial();
    
}


/////////////////////////////////////////////////////////
//
//  These subroutines are custom basic shapes used to
//    draw game objects (diamonds and squares)
//
/////////////////////////////////////////////////////////

//draw the proper number of squares (for balls, srikes, and outs)
void drawBSO(uint8_t base_x, uint8_t base_y, uint8_t num){
  
    for (uint8_t count=0; count<num; count++){
      matrix.fillRect(base_x+4*count,base_y,3,3,1);
    }
}

//draw an empty diamond of specified size
void drawDiamond(uint8_t x_top, uint8_t y_top, uint8_t offset){
  
    matrix.drawLine(x_top,y_top, x_top+offset,y_top+offset,1);
    matrix.drawLine(x_top+offset,y_top+offset,x_top,y_top+2*offset,1);
    matrix.drawLine(x_top,y_top+2*offset,x_top-offset,y_top+offset,1);
    matrix.drawLine(x_top-offset,y_top+offset,x_top,y_top,1);

}  

//draw a filled diamond of specified size
void fillDiamond(uint8_t x_top, uint8_t y_top, uint8_t offset){
    for (uint8_t start=0; start<=offset; start++){
     matrix.drawLine(x_top+start,y_top+start,x_top+start-offset,y_top+start+offset,1);
    }
}



//run through test sequence
void test() {

  Serial.println("Begining test sequence");
  drawGameState(OFF,OFF,OFF,0,0,0,0,0,0,1);
  delay(2000);

  My_Side=HOME;
  drawGameState(ON,OFF,OFF,0,0,0,1,0,1,1);
  delay(1000);

  My_Side=VISITOR;
  drawGameState(OFF,ON,OFF,0,0,0,1,2,0,2);
  delay(1000);
  
  My_Side=HOME;
  drawGameState(OFF,OFF,ON,0,0,0,3,2,1,2);
  delay(1000);
  
  My_Side=VISITOR;
  drawGameState(OFF,OFF,OFF,0,0,0,3,4,0,3);
  delay(1000);

  My_Side=-1;
  for (uint8_t b=1; b<=3; b++){
   drawGameState(OFF,OFF,OFF,b,0,0,3+2*(int(b/2)+b%2),4+2*int(b/2),b%2,3+int(b/2));
   delay(1000);
  }

  for (uint8_t s=0; s<=2; s++){
   drawGameState(OFF,OFF,OFF,0,s,0,7+2*int((s+1)/2),6+2*(int((s+1)/2)+(s+1)%2),s%2,5+int(s/2));
   delay(1000);
  }

  for (uint8_t o=0; o<=2; o++){
   drawGameState(OFF,OFF,OFF,0,0,o,11+2*int(o/2),10+2*(int(o/2)+o%2),(o+1)%2,6+int(o/2)+o%2);
   delay(1000);
  }

   drawGameState(ON,ON,ON,3,2,2,88,88,1,88);

   delay(2000);
   single_cheer(); 
   delay(2000);

   Serial.println("Test sequence complete");


  delay(5000);
  matrix.clearScreen();
  delay (2000);
}

/////////////////////////////////////////////////////////
//
//  These subroutines are used for the cheer display
//    sequence
//
/////////////////////////////////////////////////////////


//run through cheer for team x3
void cheer(){
  single_cheer();
  delay(2000);
  single_cheer();
  delay(2000);
  single_cheer();
  delay(2000);
  drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore,HScore,Half,Inning);
}

//single cheer sequence
void single_cheer(){
   matrix.clearScreen();
   matrix.setTextSize(1);    // size 1 == 8 pixels high
   matrix.setTextColor(1);   // 'lit' LEDs
   matrix.setCursor(24, 4);   
   matrix.print("LETS");
   score(VScore, HScore);
   inning(Half, Inning);
   matrix.writeScreen();
   delay(1000);

   matrix.clearScreen();
   matrix.setCursor(30, 4);   
   matrix.print("GO");
   score(VScore, HScore);
   inning(Half, Inning);
   matrix.writeScreen();
   delay(1000);
   
   matrix.clearScreen();
   print_ELITE(4);
   score(VScore, HScore);
   inning(Half, Inning);
   matrix.writeScreen();

}

//print the team name (Needed to get 5th letter on the line)
void print_ELITE(uint8_t line_offset){

  matrix.setCursor(24, line_offset);  
   matrix.print("E");
   matrix.setCursor(29, line_offset);  
   matrix.print("L");
   matrix.setCursor(32, line_offset);   
   matrix.print("I");
   matrix.setCursor(37, line_offset);   
   matrix.print("T");
   matrix.setCursor(43, line_offset);   
   matrix.print("E");

}

//writes a string using the skinny font
void writeSkinnyString(uint8_t x, uint8_t y, String message, uint8_t color, uint8_t size){
    for (uint8_t i=0; i<message.length(); i++){
      matrix.drawSkinnyChar(x,y,message[i],color,size);
      x+=3;
    }
}




/////////////////////////////////////////////////////////
//
//  Subroutine to scroll message on center screen
//
/////////////////////////////////////////////////////////

void print_message(){

  command="";

  while (inByte != 0){
    inByte = Serial.read();
    if ((inByte >= 65 && inByte <= 90) || (inByte >=97 && inByte <=122) || (inByte >= 48 &&     inByte <=57) || inByte == 43 || inByte == 61 || inByte == 63|| inByte==32) {
      command.concat(inByte);
    }
    if (inByte == 10 || inByte == 13){
        inByte=0;
        int stringlength=command.length();
        for (int c=1; c <= stringlength*6+24 ; c++){
          matrix.clearScreen();
          matrix.setTextSize(1);    // size 1 == 8 pixels high 
          matrix.setTextColor(1);   // 'lit' LEDs
          matrix.setCursor(48 - c,4); 
          matrix.print(command);
          
          matrix.fillRect(0,0,24,16,0);
          matrix.fillRect(48,0,24,16,0);
          score(VScore,HScore);
          inning(Half, Inning);
          drawHomeAway();
          
          matrix.writeScreen();
          delay(30);
        }
        drawGameState(BaseOne,BaseTwo,BaseThree,Balls,Strikes,Outs,VScore, HScore, Half, Inning);
    }
  }
}


void printScoreScreen(){
    matrix.clearScreen();

    writeSkinnyString(24,0,"MLB",1,1);
    writeSkinnyString(36,0,"GAME",1,1);
    writeSkinnyString(27,8,"SCORES",1,1);
   
    score(VScore,HScore);
    inning(Half, Inning);
    drawHomeAway();
          
    matrix.writeScreen();
  
}

void MLBRecap(){
  
    matrix.clearScreen();
    
    command="";

    while (inByte != ','){
      inByte = Serial.read();
      if (inByte >= 65 && inByte <= 90) {
        command.concat(inByte);
      }
    }
        
    writeSkinnyString(24,0,command,1,1);

    command="";
    inByte=0;
    
    while (inByte != ','){
      inByte = Serial.read();
      if (inByte >= 48 && inByte <= 57) {
        command.concat(inByte);
      }
    }
    
    writeSkinnyString(40-3*command.length(),0,command,1,1);

    command="";
    inByte=0;
    
    while (inByte != ','){
      inByte = Serial.read();
      if (inByte >= 65 && inByte <= 90) {
        command.concat(inByte);
      }
    }
        
    writeSkinnyString(24,8,command,1,1);
    
    command="";
    inByte=0;
    
    while (inByte != ','){
      inByte = Serial.read();
      if (inByte >= 48 && inByte <= 57) {
        command.concat(inByte);
      }
    }
    
    writeSkinnyString(40-3*command.length(),8,command,1,1);

    command="";
    inByte=0;

    while (inByte != ','){
      inByte = Serial.read();
      if (inByte >= 48 && inByte <= 57) {
        command.concat(inByte);
      }
    }

    writeSkinnyString(48-3*command.length(),4,command,1,1);

    inByte = Serial.read();

    if (inByte == 'T'){
      matrix.drawSkinnyChar(45,0,58,1,1);
    }
    else if (inByte=='B'){
      matrix.drawSkinnyChar(45,12,59,1,1);
    }
   
    score(VScore,HScore);
    inning(Half, Inning);
    drawHomeAway();
          
    matrix.writeScreen();

    while (Serial.available() > 0){
      inByte = Serial.read();
    }

  
}



