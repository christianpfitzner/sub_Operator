#include <SPI.h>
#include <ros.h>
#include <ohm_schroedi_mc/VelRps.h>
#include <std_msgs/UInt16.h>

#define _OK0 A2
#define _OK1 A3
#define _OK2 A4
#define _OK3 A5
#define _SS0 7
#define _SS1 6
#define _SS2 5
#define _SS3 4
#define _RESET_PIN 8

int16_t chainLeft = 0;
int16_t chainRight = 0;
int16_t flipperFrontLeft = 0;
int16_t flipperFrontRight = 0;
int16_t flipperRearLeft = 0;
int16_t flipperRearRight = 0;
int16_t command = 0;
//uint16_t timer = 0;

ros::NodeHandle nh;

void messageCb(const ohm_schroedi_mc::VelRps& msg)
{
  chainLeft = msg.chainLeft;
  chainRight = msg.chainRight;
  flipperFrontLeft = msg.flipperFrontLeft;
  flipperFrontRight = msg.flipperFrontRight;
  flipperRearLeft = msg.flipperRearLeft;
  flipperRearRight = msg.flipperRearRight;
  command = msg.command;
 // timer = millis();
}

ros::Subscriber<ohm_schroedi_mc::VelRps> sub("cmd/rps", &messageCb);
std_msgs::UInt16 pub_state;
ros::Publisher pubState("MC_state", &pub_state);

// the setup routine runs once when you press reset:
void setup() {
  pinMode(_OK0, INPUT);
  pinMode(_OK1, INPUT);
  pinMode(_OK2, INPUT);
  pinMode(_OK3, INPUT);
  pinMode(_SS0, OUTPUT);
  pinMode(_SS1, OUTPUT);
  pinMode(_SS2, OUTPUT);
  pinMode(_SS3, OUTPUT);
  pinMode(_RESET_PIN, OUTPUT);
  digitalWrite(_RESET_PIN, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.begin();
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pubState);
  digitalWrite(_SS0, HIGH);
  digitalWrite(_SS1, HIGH);
  digitalWrite(_SS2, HIGH);
  digitalWrite(_SS3, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
int16_t ReceiveData[12] = {0};
int16_t DataBrd0 [12] = {0};
int16_t DataBrd1 [12] = {0};
int16_t DataBrd2 [12] = {0};
int16_t DataBrd3 [12] = {0};

DataBrd0[1] = (-1)*chainRight; //vorne rechts
DataBrd0[2] = 0;
DataBrd1[1] = chainLeft; //vorne links
DataBrd1[2] = 0; //hinten links
DataBrd2[1] = flipperFrontLeft;
DataBrd2[2] = flipperFrontRight;
DataBrd3[1] = flipperRearLeft;
DataBrd3[2] = flipperRearRight;
/*
if ((millis() - timer) >= 2000);
{
DataBrd0[1] = 0; //vorne rechts
DataBrd0[2] = 0; //hinten rechts
DataBrd1[1] = 0; //vorne links
DataBrd1[2] = 0; //hinten links
DataBrd2[1] = 0;
DataBrd2[2] = 0;
DataBrd3[1] = 0;
DataBrd3[2] = 0;
}
*/

SendDataChain(0,DataBrd0,ReceiveData);
SendDataChain(1,DataBrd1,ReceiveData);
SendDataFlipper(2,DataBrd2,ReceiveData);
SendDataFlipper(3,DataBrd3,ReceiveData);

pub_state.data = 1;
pubState.publish(&pub_state);

nh.spinOnce();

}

void SendDataChain(uint8_t BrdID, int16_t SendArr[12], int16_t ReceiveArr[12])
{
  int8_t SendBuffer[12] = {0};
  int8_t ReceiveBuffer[12] = {0};
  int16_t VAL_MOT1 = 0;
  int16_t VAL_MOT2 = 0;
  uint8_t DIR_EN = 0;
  uint32_t CheckSum = 0;
  int8_t LByte_VAL_MOT1 = 0;
  int8_t HByte_VAL_MOT1 = 0;
  int8_t LByte_VAL_MOT2 = 0;
  int8_t HByte_VAL_MOT2 = 0;
  
  VAL_MOT1 = SendArr[1];
  VAL_MOT2 = SendArr[2];
  
  LByte_VAL_MOT1 = VAL_MOT1;
  HByte_VAL_MOT1 = (VAL_MOT1 >> 8 );
  LByte_VAL_MOT2 = VAL_MOT2;
  HByte_VAL_MOT2 = (VAL_MOT2 >> 8 );
  
  SendBuffer[0] = SendArr[0];
  SendBuffer[1] = HByte_VAL_MOT1;
  SendBuffer[2] = LByte_VAL_MOT1;
  SendBuffer[3] = HByte_VAL_MOT2;
  SendBuffer[4] = LByte_VAL_MOT2;
  SendBuffer[5] = DIR_EN;
  SendBuffer[6] = SendArr[4];
  SendBuffer[7] = SendArr[5];
  SendBuffer[8] = 0;
  SendBuffer[9] = 0;
  SendBuffer[10] = 0;
 //Serial.println(DIR_EN);
  for(uint8_t i = 0; i <= 10; i++)
  {
     CheckSum += SendBuffer[i];
  }
  
  SendBuffer[11] = (CheckSum & 0x000000FF);
  
  switch(BrdID){
    case 0:
      digitalWrite(_SS0, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));     
      } 
      digitalWrite(_SS0, HIGH);
      break;
    case 1:
      digitalWrite(_SS1, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS1, HIGH);
      break;
    case 2:
      digitalWrite(_SS2, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS2, HIGH);
      break;
    case 3:
      digitalWrite(_SS3, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS3, HIGH);
      break;
    default:
      digitalWrite(_SS0, HIGH);
      digitalWrite(_SS1, HIGH);
      digitalWrite(_SS2, HIGH);
      digitalWrite(_SS3, HIGH);
    break;
  }
}

void SendDataFlipper(uint8_t BrdID, int16_t SendArr[12], int16_t ReceiveArr[12])
{
  uint8_t SendBuffer[12] = {0};
  uint8_t ReceiveBuffer[12] = {0};
  uint16_t VAL_MOT1 = 0;
  uint16_t VAL_MOT2 = 0;
  uint8_t DIR_EN = 0;
  uint32_t CheckSum = 0;
  uint8_t LByte_VAL_MOT1 = 0;
  uint8_t HByte_VAL_MOT1 = 0;
  uint8_t LByte_VAL_MOT2 = 0;
  uint8_t HByte_VAL_MOT2 = 0;
  
  if(SendArr[1] < 1)
  {
    DIR_EN |= 1 << 0;
  }
  else 
  {
    DIR_EN &= ~(1 << 0);
  }
  
  if(SendArr[2] < 1)
  {
    DIR_EN |= 1 << 1;
  }
  else 
  {
    DIR_EN &= ~(1 << 1);
  }
  DIR_EN |= 1 << 2;
  DIR_EN |= 1 << 3;
  
  VAL_MOT1 = abs(SendArr[1]);
  VAL_MOT2 = abs(SendArr[2]);
  
  LByte_VAL_MOT1 = VAL_MOT1;
  HByte_VAL_MOT1 = (VAL_MOT1 >> 8 );
  LByte_VAL_MOT2 = VAL_MOT2;
  HByte_VAL_MOT2 = (VAL_MOT2 >> 8 );
  
  SendBuffer[0] = SendArr[0];
  SendBuffer[1] = HByte_VAL_MOT1;
  SendBuffer[2] = LByte_VAL_MOT1;
  SendBuffer[3] = HByte_VAL_MOT2;
  SendBuffer[4] = LByte_VAL_MOT2;
  SendBuffer[5] = DIR_EN;
  SendBuffer[6] = SendArr[4];
  SendBuffer[7] = SendArr[5];
  SendBuffer[8] = 0;
  SendBuffer[9] = 0;
  SendBuffer[10] = 0;
 //Serial.println(DIR_EN);
  for(uint8_t i = 0; i <= 10; i++)
  {
     CheckSum += SendBuffer[i];
  }
  
  SendBuffer[11] = (CheckSum & 0x000000FF);
  
  switch(BrdID){
    case 0:
      digitalWrite(_SS0, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));     
      } 
      digitalWrite(_SS0, HIGH);
      break;
    case 1:
      digitalWrite(_SS1, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS1, HIGH);
      break;
    case 2:
      digitalWrite(_SS2, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS2, HIGH);
      break;
    case 3:
      digitalWrite(_SS3, LOW);
      for (int i=0;i<=11;i++)
      {
        ReceiveBuffer[i]= SPI.transfer(SendBuffer[i]);
        ReceiveArr[1] = (int16_t)((ReceiveBuffer[3] << 8) | (ReceiveBuffer[4]));
        ReceiveArr[2] = (int16_t)((ReceiveBuffer[1] << 8) | (ReceiveBuffer[2]));
      } 
      digitalWrite(_SS3, HIGH);
      break;
    default:
      digitalWrite(_SS0, HIGH);
      digitalWrite(_SS1, HIGH);
      digitalWrite(_SS2, HIGH);
      digitalWrite(_SS3, HIGH);
    break;
  }
}



