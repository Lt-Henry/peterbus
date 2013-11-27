
class PeterBus:

    def __init__(self, txbuf=64, rxbuf=64):
        self.tx = [0] * txbuf
        self.ty = [0] * rxbuf

    def read(self):
        pass

    def write(self):
        pass

    def frame_avialable(self):
        pass

    def push_tx(self):
        pass

    def push_rx(self):
        pass


