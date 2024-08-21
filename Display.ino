void EnterNum(){
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(13, 80);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(3);
  carrier.display.println("Enter Number");

  carrier.display.setCursor(43, 130);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.setTextSize(5);
  carrier.display.println(G_member.toInt()); 
      

  
}
void Error(){
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(45, 80);
  carrier.display.setTextColor(ST77XX_RED);
  carrier.display.setTextSize(5);
  carrier.display.println("Error");

  carrier.display.setCursor(25, 130);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.setTextSize(2);
  carrier.display.println("Can't Compressed"); 
  delay(100);
}
void Done(){
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(50, 80);
  carrier.display.setTextColor(ST77XX_GREEN);
  carrier.display.setTextSize(5);
  carrier.display.println("Done!");

  carrier.display.setCursor(18, 130);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.setTextSize(3);
  carrier.display.println("Total Scores"); 

  carrier.display.setCursor(90, 170);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.setTextSize(3);
  carrier.display.println("100"); 
  delay(100);
  //NVIC_SystemReset();
}
void Home(){
  //carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(5, 80);
  carrier.display.setTextColor(ST77XX_RED);
  carrier.display.setTextSize(2);
  carrier.display.println("Bottle Donations");
  delay(100);

}