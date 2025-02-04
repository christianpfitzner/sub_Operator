#include <Wire.h>

#define GPIO_ENABLE_SHIELD 34

#define GPIO_DIRECTION_SHIELD1_M1 42
#define GPIO_DIRECTION_SHIELD2_M1 44
#define GPIO_DIRECTION_SHIELD3_M1 46
#define GPIO_DIRECTION_SHIELD1_M2 28 
#define GPIO_DIRECTION_SHIELD2_M2 30
#define GPIO_DIRECTION_SHIELD3_M2 32

#define GPIO_MONITOR_SHIELD1_M1 53
#define GPIO_MONITOR_SHIELD2_M1 51
#define GPIO_MONITOR_SHIELD3_M1 49
#define GPIO_MONITOR_SHIELD1_M2 3 // original -> 26, but pull-up not compatible with Maxon monitor output
#define GPIO_MONITOR_SHIELD2_M2 24
#define GPIO_MONITOR_SHIELD3_M2 22

#define I2C_MOTOR_SHIELD1 65
// to be defined
#define I2C_MOTOR_SHIELD2 0
// to be defined
#define I2C_MOTOR_SHIELD3 0

bool _enable = false;

// Timeout in milliseconds
#define MOTOR_TIME_OUT 500
unsigned long _time_last_cmd = 0;

int _i2c_address[3] = { I2C_MOTOR_SHIELD1, I2C_MOTOR_SHIELD2, I2C_MOTOR_SHIELD3 };
int _gpio_direction_pins[6] = {GPIO_DIRECTION_SHIELD1_M1,
                               GPIO_DIRECTION_SHIELD2_M1,
                               GPIO_DIRECTION_SHIELD3_M1,
                               GPIO_DIRECTION_SHIELD1_M2,
                               GPIO_DIRECTION_SHIELD2_M2,
                               GPIO_DIRECTION_SHIELD3_M2};
unsigned char _uart_buffer[12];
volatile unsigned short _ticks[6] = {0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(GPIO_ENABLE_SHIELD, OUTPUT);
  
  pinMode(GPIO_DIRECTION_SHIELD1_M1, OUTPUT);
  pinMode(GPIO_DIRECTION_SHIELD1_M2, OUTPUT);
  
  pinMode(GPIO_MONITOR_SHIELD1_M1, INPUT);
//  pinMode(GPIO_MONITOR_SHIELD2_M1, INPUT);
//  pinMode(GPIO_MONITOR_SHIELD3_M1, INPUT);
  pinMode(GPIO_MONITOR_SHIELD1_M2, INPUT);
 // pinMode(GPIO_MONITOR_SHIELD2_M2, INPUT);
//  pinMode(GPIO_MONITOR_SHIELD3_M2, INPUT);

  
  digitalWrite(GPIO_ENABLE_SHIELD, LOW);
  digitalWrite(GPIO_DIRECTION_SHIELD1_M1, LOW);
  digitalWrite(GPIO_DIRECTION_SHIELD1_M2, LOW);
  
  attachInterrupt(GPIO_MONITOR_SHIELD1_M1, onEncoderTickS1M1, RISING);
  attachInterrupt(GPIO_MONITOR_SHIELD1_M2, onEncoderTickS1M2, RISING);
  
  Wire.begin();
}

void onEncoderTickS1M1()
{
  _ticks[0]++;
}

void onEncoderTickS1M2()
{
  _ticks[1]++;
}

void setRpm(int i2c_address, short val)
{
  byte outputData[3];
  outputData[0] = 0x3F;
  outputData[1] = (val & 0xFF00) >> 8;
  outputData[2] = (val & 0x00FF);
  Wire.beginTransmission(i2c_address);  
  for(int i=0; i<3; i++)
    Wire.write(outputData[i]);
  Wire.endTransmission();
}

void interpreteBuffer()
{
  short val[6];
  bool dir[6];
  _enable = false;
  for(int i=0; i<6; i++)
  {
    short upperByte = _uart_buffer[2*i];
    short lowerByte = _uart_buffer[2*i+1];
    val[i] = (upperByte << 8) | lowerByte;
    // enable all motors, if at least one motor is driven 
    _enable |= (abs(val[i])>50);
    dir[i] = (val[i]>0);
    if(!dir[i]) val[i] = -val[i];
  }
  
  if(_enable)
    digitalWrite(GPIO_ENABLE_SHIELD, HIGH);
  else
    digitalWrite(GPIO_ENABLE_SHIELD, LOW);
  
  digitalWrite(GPIO_DIRECTION_SHIELD1_M1, dir[0]);
  digitalWrite(GPIO_DIRECTION_SHIELD1_M2, dir[1]);
  
  for(int i=0; i<3; i++)
  {
    setRpm(_i2c_address[i], val[i]);
  }
}

#define UART_WAIT_FOR_START 0
#define UART_RECEIVING 1
#define UART_SUCCESS 2
int uart_handler()
{
  static int state = UART_WAIT_FOR_START;
  static int cnt = 0;
  byte inByte = 0;         // incoming serial byte
  
  if (Serial.available() > 0)
  {
    // get incoming byte:
    inByte = Serial.read();

    switch(state)
    {
      case UART_WAIT_FOR_START:
        cnt = 0;
        if(inByte=='#')
        {
          state = UART_RECEIVING;
        }
        break;
      case UART_RECEIVING:
        if(cnt<=11)
        {
          _uart_buffer[cnt++] = inByte;
        }
        else
        {
          if(inByte=='$')
          {
            //Serial.write("Command received\n");
            _time_last_cmd = millis();
            
            unsigned short ticks[6];
            for(int i=0; i<6; i++)
            {
              ticks[i] = _ticks[i];
              _ticks[i] = 0;
            } 
            Serial.write((unsigned char*)ticks, 12);

            state = UART_SUCCESS;
          }
          else
          {
            //Serial.write("Serial communication error\n");
            state = UART_WAIT_FOR_START;
          }
        }
        break;
    }
  }
  
  int retval = state;
  if(state==UART_SUCCESS)
    state = UART_WAIT_FOR_START;
  
  return retval;
}

void loop()
{
  if(uart_handler()==UART_SUCCESS)
  {
    interpreteBuffer();
  }
  
  // protocol time out check: Stop motors immediatly, if connection is bad
  unsigned long elapsed = millis() - _time_last_cmd;
  if(elapsed>250 && _enable)
  {
    digitalWrite(GPIO_ENABLE_SHIELD, LOW);
    for(int i=0; i<3; i++)
      setRpm(_i2c_address[i], 0);
    _enable = false;
  }
}
