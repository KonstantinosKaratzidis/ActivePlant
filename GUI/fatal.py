from PyQt5.QtWidgets import QDialog, QApplication
from generated.fatal_message import Ui_FatalMessage

class FatalMessage(QDialog):
    def __init__(self, msg, *args):
        super().__init__(*args)
        self._dialog = Ui_FatalMessage()
        self._dialog.setupUi(self)
        self._dialog.message.setText(msg)
        self._dialog.okButton.clicked.connect(QApplication.instance().quit)
