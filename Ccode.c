#include <kamavr.h>
unsigned int getadc(int chan);
void print_string(char* str);
void build_cgram();
int voltage_to_cg(int val, int cnt);
void display_volt_cnt(int input);
int integer_array[100];
int decimal_array[100];
int cg_array[100];
int speed;
unsigned char key;


//------------------- MAIN FUNCTION---------------------------------



int main(void)
{
//	Enanbling ADC
ADCSR = ADCSR | (1<< (ADEN));

//  set prescaler frequency
ADCSR |= 0b00000111;    // ADPS2, ADPS1, ADPS0 = 1

//	set single conversion
ADCSR&= ~(1<<(ADFR)); //ADFR = 0
init_lcd();

//	calling the saved cgram
build_cgram();


while (1)
{
	init_lcd();



//-------- CODES FOR :  SPEED SETTING   AND PRESS B TO CONTINUE ---------



//waiting until speed is selected by using keypad
do
{
write_lcd(0,0x80);
print_string("SET SPEED");
key=readkey();
speed=key;
}while(key>9);

//display the speed
write_lcd(0,0xC0);
write_lcd(1,0x30+speed);
delay_ms(250);
//display message "press B"
init_lcd();
write_lcd(0,0x80);
print_string("PRESS B");


//----------- CODES FOR :  to show 	SETTING SAMPLING RATE  --------



//set the delay, the bigger the selected value of keypad, the slower the sampling rate
int delay=(10+speed)*20;
	do{
	key=readkey();
	}while(key!=11);

init_lcd();


//------------- CODES :  To show S:Speed     on the LCD   ---------


//point to lower most left loaction of LCD
write_lcd(0,0xC0);
//write letter "S"
write_lcd(1,0x53);
//write symbol ":"
write_lcd(1,0x3A);
//write the speed value
write_lcd(1,0x30 + speed);



//------- CODES : TO SHOW  ""STRIPS"" TO THE FIRST 13 BLOCKS FROM LEFT SIDE ON LCD ----


//point to upper most left location of LCD for bar graph
write_lcd(0,0x80);
//the bar graph on LCD initially shows strips
for (int i=0; i<13;i++)
write_lcd(1,0x2D);





//-----------ARRAY INITIALIZATION------------------

for (int i=0; i<100; i++)
{
integer_array[i] =0x30; //making array of zeros
decimal_array[i] =0x30; //making array of zeros
cg_array[i] =0x20; //making array of space characters
}

//initialize counter
int count=0;


//--- CONTINUOUSLY GET THE INPUT UNTILL KEYPAD C IS NOT PRESSED ------

do
{
key=readkey();

count++;
	int voltage = getadc(0)*0.049; // (5/1023) *10
	int integer = voltage/10;
	int decimal = voltage%10;


//when the recording exceed the 100 of array, discard the oldest data,
//shift the data to the previous index,
  if (count>100)
  {
	for (int n=0;n<100-1;n++)
	{
	integer_array[n]=integer_array[n+1];
	decimal_array[n]=decimal_array[n+1];
	cg_array[n]=cg_array[n+1];
	}
    //make counte equal 100 to save the new 100 in the last index
	count=100;
  }

//the last index for new 100
	integer_array[count-1]=0x30 + integer;
	decimal_array[count-1]=0x30 + decimal;


//using the current voltage to get the CGRAM and save it in cg_array
	voltage_to_cg(voltage, count);
 





//------------CODE TO DISPLAY THE FINAL BAR GRAPH ------------------------



//the first 13 samples(16 slots on lcd, rightmost 3 are used for displaying voltage)
 if(count<14)
 { 
	for(int n=0;n<count;n++)
	{
	write_lcd(0,0x8C - (count-n-1)); //showing for the first 13 samples of array

	write_lcd(1,cg_array[n]);
	}
 } 


//display the bar graph after the 13th to 100th sample value
//shifting the bar as inserting new 100 to array
 else
 {
	for(int n=0; n<13; n++)
	{
	write_lcd(0,0x8C-n); // showing for the shifted samples after 13th sample values

	write_lcd(1, cg_array[count-n-1]);
	}
 }



//----------  CODE TO DISPLAY VOLTAGE IN DIGITS AND INDEX NUMBER -----------


//display first number, komma sign, decimal number
 display_volt_cnt(count);
 delay_ms(delay);
}
 while(key!=12);     //  stop this activity when button C is pressed


}
}



//-----------  OTHER     FUNCTIONS   	 ----------



//-------OBTAIN ADC VALUE FROM POTENTIOMETER --------

unsigned int getadc(int channel)
{
ADMUX = channel; //selecting channel
ADCSR |= (1<< (ADSC)); //start conversion
while (ADCSR & (1<<(ADSC))) //waiting until conversion finish
;
return ADC;
}



//--------PRINT STRING -----------------

void print_string(char* string)
{
int i = 0;
while(string[i]) //display each character of the string
write_lcd(1,string[i++]);
}




//-------CGRAM SETUP------------------

void build_cgram()
{
//point to the first cgram addresss
write_lcd(0,0x40);
//write from top line to bottom line
for(int i=0;i<7;i++)
write_lcd(1,0); //7 lines blank
write_lcd(1,0b00011111); //1 line full
for(int i=0;i<6;i++)
write_lcd(1,0); //6 lines blank
for(int i=0;i<2;i++)
write_lcd(1,0b00011111); //2 lines full
for(int i=0;i<5;i++)
write_lcd(1,0); //5 lines blank
for(int i=0;i<3;i++)
write_lcd(1,0b00011111); //3 lines full
for(int i=0;i<4;i++)
write_lcd(1,0); //4 lines blank
for(int i=0;i<4;i++)
write_lcd(1,0b00011111); //4 lines full
for(int i=0;i<3;i++)
write_lcd(1,0); //3 lines blank
for(int i=0;i<5;i++)
write_lcd(1,0b00011111); //5 lines full
for(int i=0;i<2;i++)
write_lcd(1,0); //2 lines blank
for(int i=0;i<6;i++)
write_lcd(1,0b00011111); //6 lines full
for(int i=0;i<1;i++)
write_lcd(1,0); //1 lines blank
for(int i=0;i<7;i++)
write_lcd(1,0b00011111); //7 lines full
for(int i=0;i<8;i++)
write_lcd(1,0b00011111); //8 lines full
}




//----------- VOLTAGE TO RESPECTIVE BAR CONVERSION  ---------------


int voltage_to_cg(int val, int cnt)
{ //There are 8 lines for 5V, split the voltage by 6 to create 8 sections
//In this case the voltage is multiplied by 10, therefore the range is from 0 - 50
//the index of cg_array start from 0
if (val<=6)
{
cg_array[cnt-1]=0x20; //blank, cgram 0
}
else if(val<=12)
{
cg_array[cnt-1]=0x00; //1 line, ASCII 0 = cgram address 1
}
else if(val<=18)
{
cg_array[cnt-1]=0x01; //2 lines, ASCII 1 = cgram address 2
}
else if(val<=24)
{
cg_array[cnt-1]=0x02; //3 lines, ASCII 2 = cgram address 3
}
else if(val<=30)
{
cg_array[cnt-1]=0x03; //4 lines, ASCII 3 = cgram address 4
}
else if(val<=36)
{
cg_array[cnt-1]=0x04; //5 lines, ASCII 4 = cgram address 5
}
else if(val<=42)
{
cg_array[cnt-1]=0x05; //6 lines, ASCII 5 = cgram address 6
}
else if(val<=48)
{
cg_array[cnt-1]=0x06; //7 lines, ASCII 6 = cgram address 7
}
else
{
cg_array[cnt-1]=0x07; //8 lines(full line), ASCII 7 = cgram address 8
}
return cg_array[cnt-1];
}




//---- SHOW THE VOLTAGE AND INDEX AT THE UPPER AND LOWER RIGHT OF LCD  ---



void display_volt_cnt(int input)
{
//display the voltage
write_lcd(0,0x8D);
write_lcd(1,integer_array[input-1]);
write_lcd(1,0x2E);
write_lcd(1,decimal_array[input-1]);
//display the index, 
write_lcd(0,0xCD);
write_lcd(1,0x30+((input-1)/100));
write_lcd(1,0x30+((input-1)%100 /10));
write_lcd(1,0x30+((input-1)%10));

}
