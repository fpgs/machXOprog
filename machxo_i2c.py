# The MIT License (MIT)
#
# Author(s): Greg Steiert
# Copyright (c) 2018 Lattice Semiconductor Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
`machxo_i2c`
====================================================
An I2C programming utility for the MachXO2/3.
* Author(s): Greg Steiert
"""

from adafruit_bus_device.i2c_device import I2CDevice

# ISC_ERASE bit assignments
ERASE_SRAM = 1 << 16        
ERASE_FEATURE_ROW = 1 << 17
ERASE_CONGFIG_FLASH = 1 << 18
ERASE_UFM = 1 << 19

class MachXOi2c:
    """Programming utility for MachXO2/3 through I2C"""
    
    def init(self, i2c_bus, i2c_addr=0x40):
        """Assign i2c bus and lock it for use"""
        self.i2c_dev = I2CDevice(i2c_bus, i2c_addr)
        while not self.i2c_dev.i2c.try_lock():
            pass
    
    def kill(self):
        self.i2c_dev.i2c.unlock()
    
    def read_device_id(self):
        """Read and return device ID"""
        device_id = bytearray(4)
        self.i2c_dev.write(bytes([0xE0, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(device_id)
        return device_id
    
    def read_user_code(self):
        """Read and return user code"""
        user_code = bytearray(4)
        self.i2c_dev.write(bytes([0xC0, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(user_code)
        return user_code
    
    def read_status(self):
        """Read and return status register"""
        status = bytearray(4)
        self.i2c_dev.write(bytes([0x3C, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(status)
        return status
    
    def read_feature_row(self):
        """Read and return feature row"""
        feature_row = bytearray(8)
        self.i2c_dev.write(bytes([0xE7, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(feature_row)
        return feature_row
    
    def read_feature_bits(self):
        """Read and return feature bits"""
        feature_bits = bytearray(2)
        self.i2c_dev.write(bytes([0xFB, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(feature_bits)
        return feature_bits
    
    def read_flash(self):
        """Read one page from flash"""
        read_data = bytearray(16)
        self.i2c_dev.write(bytes([0x73, 0x00, 0x00, 0x01]), stop=False)
        self.i2c_dev.readinto(read_data)
        return read_data
    
    def read_ufm(self):
        """Read one page from UFM"""
        read_data = bytearray(16)
        self.i2c_dev.write(bytes([0xCA, 0x00, 0x00, 0x01]), stop=False)
        self.i2c_dev.readinto(read_data)
        return read_data

    def read_otp_fuses(self):
        """Read and return status register"""
        ucfsucfs = bytearray(1)
        self.i2c_dev.write(bytes([0xFA, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(ucfsucfs)
        return ucfsucfs

    def erase_ufm(self):
        """Erase only UFM"""
        self.i2c_dev.write(bytes([0xCB, 0, 0, 0]), stop=True)
    
    def erase(self, flags):
        """Erase blocks identified in flags"""
        command = bytes([
            0x0E, 
            (0xFF & (flags >> 16)), 
            (0xFF & (flags >> 8)),
            (0xFF & (flags >> 0)),
        ])
        self.i2c_dev.write(command, stop=True)
    
    def enable_config_transparent(self):
        self.i2c_dev.write(bytes([0x74, 0x08, 0]), stop=True)

    def enable_config_offline(self):
        self.i2c_dev.write(bytes([0xC6, 0x08, 0]), stop=True)

    def check_busy(self):
        """Return True if busy"""
        busy_byte = bytearray(1)
        self.i2c_dev.write(bytes([0xF0, 0, 0, 0]), stop=False)
        self.i2c_dev.readinto(busy_byte)
        if (busy_byte[0] & 0x80):
            return True
        else: 
            return False

    def wait_busy(self):
        while self.check_busy():
            pass
    
    def reset_config_addr(self):
        """Set page address to beginning of configuration sector"""
        self.i2c_dev.write(bytes([0x46, 0, 0, 0]), stop=True)

    def reset_ufm_addr(self):
        """Set page address to beginning of UFM sector"""
        self.i2c_dev.write(bytes([0x47, 0, 0, 0]), stop=True)

    def set_config_addr(self, page):
        """Set page address of configuration sector"""
        command = bytes([
            0xB4, 0x00, 0x00, 0x00,
            0x00, 0x00, (0xFF & (page >> 8)), (0xFF & page),
        ])
        self.i2c_dev.write(command, stop=True)

    def set_ufm_addr(self, page):
        """Set page address of UFM sector"""
        command = bytes([
            0xB4, 0x00, 0x00, 0x00,
            0x40, 0x00, (0xFF & (page >> 8)), (0xFF & page),
        ])
        self.i2c_dev.write(command, stop=True)

    def program_page(self, page_data):
        if (len(page_data) == 16):
            command = bytes([
                0x70, 0x00, 0x00, 0x01,
                page_data[0], page_data[1], page_data[2], page_data[3], 
                page_data[4], page_data[5], page_data[6], page_data[7], 
                page_data[8], page_data[9], page_data[10], page_data[11], 
                page_data[12], page_data[13], page_data[14], page_data[15],
            ])
            self.i2c_dev.write(command, stop=True)
        else:
            print("wrong number of elements in write array")
    
    def program_done(self):
        self.i2c_dev.write(bytes([0x5E, 0x00, 0x00, 0x00]), stop=True)

    def refresh(self):
        """Reconfigure device"""
        self.i2c_dev.write(bytes([0x79, 0, 0]), stop=True)
    
    def wakeup(self):
        """No opperation, wakeup device"""
        self.i2c_dev.write(bytes([0xFF, 0xFF, 0xFF, 0xFF]), stop=True)
    
    def load_jed(self, filename):
        jed_file = open(filename, 'r')
        line = jed_file.readline().rstrip()
        while line:
            if line[0] == "L":
                page_addr = int(line[1:])
                if page_addr == 0:
                    self.reset_config_addr()
                    line = jed_file.readline().rstrip()
                    count = 0
                    while len(line) == 128:
                        page_data = [ 
                            int(line[0:8], 2),
                            int(line[8:16], 2),
                            int(line[16:24], 2),
                            int(line[24:32], 2),
                            int(line[32:40], 2),
                            int(line[40:48], 2),
                            int(line[48:56], 2),
                            int(line[56:64], 2),
                            int(line[64:72], 2),
                            int(line[72:80], 2),
                            int(line[80:88], 2),
                            int(line[88:96], 2),
                            int(line[96:104], 2),
                            int(line[104:112], 2),
                            int(line[112:120], 2),
                            int(line[120:128], 2),
                            ]
                        self.program_page(page_data)
# It takes >400us to load command +128 bits at 400kHz
# You shouldn't need to check the busy flag with I2C
#                        self.wait_busy()
                        count += 1
                        line = jed_file.readline().rstrip()
                    if line[0] == "*":
                        print("wrote ", count, " pages")
                    else:
                        print("bad page data format")
            line = jed_file.readline().rstrip()
        jed_file.close()

    def load_hex(self, filename):
        hex_file = open(filename, 'r')
        count = 0
        self.reset_config_addr()
        line = hex_file.readline().rstrip()
        while line:
            if len(line) == 32:
                page_data = [ 
                    int(line[0:2], 16),
                    int(line[2:4], 16),
                    int(line[4:6], 16),
                    int(line[6:8], 16),
                    int(line[8:10], 16),
                    int(line[10:12], 16),
                    int(line[12:14], 16),
                    int(line[14:16], 16),
                    int(line[16:18], 16),
                    int(line[18:20], 16),
                    int(line[20:22], 16),
                    int(line[22:24], 16),
                    int(line[24:26], 16),
                    int(line[26:28], 16),
                    int(line[28:30], 16),
                    int(line[30:32], 16),
                    ]
                self.program_page(page_data)                       
# It takes >400us to load command +128 bits at 400kHz
# You shouldn't need to check the busy flag with I2C
#                self.wait_busy()
                count += 1
            else:
                print("expected 32 bytes, got ", len(line), " at line ", count) 
            line = hex_file.readline().rstrip()
        print("wrote ", count, " pages")
        hex_file.close()
        self.wait_busy()