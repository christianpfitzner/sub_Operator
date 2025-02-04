#command.msg
#author: Martin Fees
#This message is used to send commands to the inspector node.
Header header	 		#message header
bool moveHome			#true when inspector should move to home position
bool movePark			#true when inspector should move to park position
float32[2] speed2D 		#speed in cartesian coordinate system, 0: FWD/BWD - direction 1: UP/DOWN - direction [mm/s]
float32[2] speedAxis	#speed of additional axis for nivellation
float32 speedTCP		#speed in FWD -direction of the tool-center-point