import utime

from trezor import io, utils

if not utils.EMULATOR:
    MOTOR_CTL = io.MOTOR()

    def vibrate():
        # start forward
        MOTOR_CTL.ctrl(2)
        # sleep
        utime.sleep_ms(25)
        # brake
        MOTOR_CTL.ctrl(3)
        # coast
        MOTOR_CTL.ctrl(0)

else:

    def vibrate():
        pass
