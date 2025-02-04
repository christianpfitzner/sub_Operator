/** @brief arduino firmware for escon_controller
 * 
 *  @author Michael Schmidpeter
 */

#define DISABLE_M_R         digitalWrite(EN_R,LOW)
#define DISABLE_M_L         digitalWrite(EN_L,LOW)
#define ENABLE_M_R          digitalWrite(EN_R,HIGH)
#define ENABLE_M_L          digitalWrite(EN_L,HIGH)
#define M_R_FORWARD         digitalWrite(ROT_R, LOW)
#define M_R_BACKWARD        digitalWrite(ROT_R, HIGH)
#define M_L_FORWARD         digitalWrite(ROT_L, LOW)
#define M_L_BACKWARD        digitalWrite(ROT_L, HIGH)
#define LED_ON              digitalWrite(13, HIGH)
#define LED_OFF             digitalWrite(13, LOW)
#define LED(x)              digitalWrite(13, (x))

namespace{
int PWM_R = 9;
int PWM_L = 5;
int EN_R  = 7;
int EN_L  = 3;
int ROT_R = 6;
int ROT_L = 2;

const uint8_t CRC8MASK = 0xA6;

const int PWM_MIN = 20;   //10%
const int PWM_MAX = 231;  //90%
const int32_t RPM_MAX = 7580;

namespace serial{
const uint8_t TRIGGER = 0xff;

enum DATA_FORMAT{
    MOTOR_R_LOW = 0,
    MOTOR_R_HIGH,    //1 = forward, 0 = backward
    MOTOR_L_LOW,
    MOTOR_L_HIGH,    //1 = forward, 0 = backward
    CRC,
    DATA_SIZE
};
}

}

uint8_t serialData[serial::DATA_SIZE];
bool compute = false;

int led = 0;
void setup() {

    Serial.begin(9600);
    Serial.setTimeout(1000);

    pinMode(PWM_R, OUTPUT);
    pinMode(PWM_L, OUTPUT);
    pinMode(EN_R, OUTPUT);
    pinMode(EN_L, OUTPUT);
    pinMode(ROT_R, OUTPUT);
    pinMode(ROT_L, OUTPUT);
    pinMode(13,OUTPUT);

    analogWrite(PWM_R,PWM_MIN);
    analogWrite(PWM_L,PWM_MIN);
    ENABLE_M_L;
    ENABLE_M_R;
    M_L_FORWARD;
    M_L_BACKWARD;

    for (int i = 0; i < serial::DATA_SIZE; ++i)
    {
      serialData[i] = 0;
    }

    LED_OFF;
}

void toBitStream(uint8_t* data_stream, uint8_t* data , unsigned int data_length)
{
   for(unsigned int i = 0; i < data_length; ++i)
      for(unsigned int k = 0; k < 8; ++k)
         data_stream[i*8+k] = (data[i] >> (7-k)) & 1;
}

unsigned int computeCRC(uint8_t* data, unsigned int length)
{
   unsigned int num_databits = length * 8;
   uint8_t* data_stream = new uint8_t[num_databits];
   toBitStream(data_stream, data, length);

   for(unsigned int i = 0; i < num_databits; ++i)
   {
      printf("%d",data_stream[i]);
   }
   printf("\n");


   uint8_t crc8 = 0; /* Schieberegister */

   int i;
   for (i = 0; i < num_databits; ++i)
   {
       if (((crc8 & 0x80) ? 1 : 0) != data_stream[i])
          crc8 = (crc8 << 1) ^ CRC8MASK;
       else
          crc8 <<= 1;
   }
   delete[] data_stream;
   return crc8;
}

void loop()
{
    char trigger = 1;
    uint8_t pwm_r = 0;
    uint8_t pwm_l = 0;
    compute = false;
    //get trigger
    do{
        Serial.readBytes(&trigger,1);
    }while((uint8_t)trigger != serial::TRIGGER);
    //read data
    int ret = Serial.readBytes((char*)serialData,serial::DATA_SIZE);
    if(ret != serial::DATA_SIZE)
    {
        serialData[serial::MOTOR_R_LOW ] = 0;
        serialData[serial::MOTOR_R_HIGH] = 0;
        serialData[serial::MOTOR_L_LOW]  = 0;
        serialData[serial::MOTOR_L_HIGH] = 0;
        compute = false;
    }
    else
    {
      uint8_t ret = (uint8_t)computeCRC(serialData,serial::DATA_SIZE-1);
      if(serialData[serial::CRC] == ret)
      {
         compute = true;
      }
      // Serial.write(serialData,serial::DATA_SIZE);
    }

    if(compute)
    {
      int16_t mot_r = serialData[serial::MOTOR_R_HIGH];
      mot_r = mot_r << 8;
      mot_r += serialData[serial::MOTOR_R_LOW];
      int16_t mot_l = serialData[serial::MOTOR_L_HIGH];
      mot_l = mot_l << 8;
      mot_l += serialData[serial::MOTOR_L_LOW];

      //get direction
      (mot_r >= 0) ? M_R_FORWARD : M_R_BACKWARD;
      (mot_l >= 0) ? M_L_FORWARD : M_L_BACKWARD;

      //-- get pwm values --

      //   sacle down to PWM_MAX - PWM_MIN and add PWM_MIN
      uint32_t scale = PWM_MAX - PWM_MIN;
      uint32_t tmp = ((abs(mot_r) * (int32_t)10000) / RPM_MAX) * scale;
      pwm_r = tmp / (int32_t)10000 + PWM_MIN;
   
      tmp = ((abs(mot_l) * (int32_t)10000) / RPM_MAX) * scale;
      pwm_l = tmp / (int32_t)10000 + PWM_MIN;
   
      //Serial.write(&pwm_r,1);
      //Serial.write(&pwm_l,1);
   
      //prove min max
      if(pwm_r < PWM_MIN)
         pwm_r = PWM_MIN;
      if(pwm_r > PWM_MAX)
         pwm_r = PWM_MAX;

      if(pwm_l < PWM_MIN)
         pwm_l = PWM_MIN;
      if(pwm_l > PWM_MAX)
         pwm_l = PWM_MAX;

      analogWrite(PWM_R, pwm_r);
      analogWrite(PWM_L, pwm_l);
      
      Serial.write(&pwm_r,1);
      Serial.write(&pwm_l,1);
    
   }
}
