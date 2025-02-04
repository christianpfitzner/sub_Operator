#DriveState.msg
#author: Martin Fees
#This message is used to transmit and receive state data of drives
Header header         #Header of the message
float64 position      #Position of the actuator in (rad)
float64 velocity      #Velocity of the actuator in (rad/sec)
float64 acceleration  #Acceleration of the actuators in (rad/sec^2)