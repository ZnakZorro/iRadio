/*
void myfunction(void) {
PGM_P xyz = PSTR("Store this string in flash");
const char * abc = PSTR("Also Store this string in flash");
}
*/
 
/*
String splitStringIndex(String data, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)=='\n' || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
*/
/*
String splitStringIndex(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

*/

/*
// display a string on multiple lines, keeping words intact where possible
void printwords(const char *msg, int xloc, int yloc) {
   int dspwidth = u8g2.getDisplayWidth(); // display width in pixels
   int strwidth = 0;  // string width in pixels
   char glyph[2]; glyph[1] = 0;
   for (const char *ptr = msg, *lastblank = NULL; *ptr; ++ptr) {
      while (xloc == 0 && *msg == ' ')
         if (ptr == msg++) ++ptr; // skip blanks at the left edge
      glyph[0] = *ptr;
      strwidth += u8g2.getStrWidth(glyph); // accumulate the pixel width
      if (*ptr == ' ')  lastblank = ptr; // remember where the last blank was
      else ++strwidth; // non-blanks will be separated by one additional pixel
      if (xloc + strwidth > dspwidth) { // if we ran past the right edge of the display
         int starting_xloc = xloc;
         while (msg < (lastblank ? lastblank : ptr)) { // print to the last blank, or a full line
            glyph[0] = *msg++;
            xloc += u8g2.drawStr(xloc, yloc, glyph); }
         strwidth -= xloc - starting_xloc; // account for what we printed
         yloc += u8g2.getMaxCharHeight(); // advance to the next line
         xloc = 0; lastblank = NULL; } }
   while (*msg) { // print any characters left over
      glyph[0] = *msg++;
      xloc += u8g2.drawStr(xloc, yloc, glyph); 
  } 
}

*/
