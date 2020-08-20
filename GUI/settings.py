from PyQt5.QtWidgets import QDialog
from PyQt5.QtCore import Qt, pyqtSlot, pyqtSignal, QObject
from generated.settings import Ui_Settings

class Settings(QDialog):

    # target_moisture, water_interval
    settingsChanged = pyqtSignal(int, int)
    def __init__(self, target_moisture, water_interval, *args):
        super().__init__(*args)
        self._dialog = Ui_Settings()
        self._dialog.setupUi(self)

        self.okButton = self._dialog.okButton
        self.cancelButton = self._dialog.cancelButton
        self.intervalBox = self._dialog.waterIntervalBox
        self.moistureSlider = self._dialog.targetMoistureSlider

        self.intervalBox.setValue(water_interval)
        self.moistureSlider.setValue(target_moisture)

        self.cancelButton.clicked.connect(self.close)
        self.okButton.clicked.connect(self._settingsChanged)

    def _settingsChanged(self):
        target_moisture = self.moistureSlider.value()
        water_interval = self.intervalBox.value()
        self.settingsChanged.emit(target_moisture, water_interval)
