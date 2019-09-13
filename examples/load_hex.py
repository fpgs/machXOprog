import board
import busio
import machxo_i2c
from machxo_i2c import MachXOi2c 
i2c = busio.I2C(board.SCL, board.SDA)
machxo = MachXOi2c()
machxo.init(i2c)
print("machxo initialized")
machxo.enable_config_offline()
print("offline configuration enabled")
machxo.erase(machxo_i2c.ERASE_CONGFIG_FLASH)
machxo.wait_busy()
print("device erased")
print("loading tiny256.hex")
machxo.load_hex('tiny256.hex')
machxo.program_done()
print("programming done")
print("Status: ", 
      [hex(data_byte) for data_byte in machxo.read_status()])
machxo.refresh()
print("exited configuration mode")
machxo.kill()