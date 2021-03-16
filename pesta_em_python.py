import PyCmdMessenger
import numpy as np
from scipy import ndimage

s_c = PyCmdMessenger.ArduinoBoard('/dev/ttyACM0', baud_rate = 115200)

commands =[["kAcknowlegde","s"],
           ["kError","s"],
           ["kGetVelocities","ff"],
	   ["kSendDataSensors","ffffff"]]

cmd = PyCmdMessenger.CmdMessenger(s_c, commands)

map = np.zeros((250,350), dtype=float)
map[1:249,1:349] = 1
map[73:175,79:144] = 0
map[73:175,214:279] = 0


distance_map = ndimage.distance_transform_edt(map)
gradient_mapx = np.gradient(distance_map, axis=0)
gradient_mapy = np.gradient(distance_map, axis=1)


#print(distance_map)
#print(gradient_mapx)
#print(gradient_mapy)

while True:
	cmd.send("kSendDataSensors")
	sensor = cmd.receive()
	encoderL = sensor[0]
	encoderR = sensor[1]
	irleft = sensor[2]
	irright = sensor[3]
	irfront = sensor[4]
	irback = sensor[5]

