import board
import busio
import machxo_i2c
from machxo_i2c import MachXOi2c 
i2c = busio.I2C(board.SCL, board.SDA)
machxo = MachXOi2c()
machxo.init(i2c)

machxo.enable_config_offline()

machxo.erase(machxo_i2c.ERASE_CONGFIG_FLASH)
machxo.wait_busy()

machxo.load_jed('xo3starter.jed')

machxo.program_done()

print("Status: ", 
      [hex(data_byte) for data_byte in machxo.read_status()])

machxo.refresh()

machxo.kill()