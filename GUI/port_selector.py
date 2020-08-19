from generated.port_selector import Ui_portSelector
from PyQt5.QtWidgets import QDialog, QApplication, QDialogButtonBox
from PyQt5.QtCore import Qt, pyqtSlot, pyqtSignal, QObject

class PortSelector(QDialog):
    # emited when ok is clicked and port has been selected
    portSelected = pyqtSignal(str)
    # emited when cancel is clicked
    canceled = pyqtSignal()

    def __init__(self, ports, *args):
        """Pass a list of serial port names"""
        super().__init__()
        self.ports = ports
        self.setWindowFlags(Qt.Dialog | Qt.Window)
        self._dialog = Ui_portSelector()
        self._dialog.setupUi(self)

        options = self._dialog.portsComboBox
        for port in ports:
            options.addItem(port)
        options.currentIndexChanged.connect(self.currentSelectionChanged)
        
        self.ok_button = self._dialog.buttonBox.button(QDialogButtonBox.Ok)
        self.ok_button.setEnabled(False)
        self.ok_button.clicked.connect(self._okClicked)

        self.cancel_button = self._dialog.buttonBox.button(QDialogButtonBox.Cancel)
        self.cancel_button.clicked.connect(self.canceled)

        self.currentSelectedPort = None

    def currentSelectionChanged(self, index):
        self.currentSelectedPort = self.ports[index]
        self.ok_button.setEnabled(True)

    def _okClicked(self):
        self.portSelected.emit(self.currentSelectedPort)

if __name__ == "__main__":
    import sys
    app = QApplication(sys.argv)
    selector = PortSelector(["/dev/ttyUSB0"])
    selector.portSelected.connect(QApplication.instance().quit)
    selector.canceled.connect(QApplication.instance().quit)
    selector.show()
    app.exec_()
