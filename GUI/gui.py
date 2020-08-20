from PyQt5.QtWidgets import QApplication, QMainWindow
from generated.main_window import Ui_MainWindow
from port_selector import PortSelector
from fatal import FatalMessage
from plant import Plant, import_plants
from proto import PlantConnection, ResponseTimeout
from settings import Settings
import time
from serial import Serial
from serial.tools.list_ports import comports
from sys import argv

MAX_MOISTURE = 1024
MAX_WATER_LEVEL = 4

class MainWindow(QMainWindow):
    def __init__(self, *args):
        super().__init__(*args)
        self.conn = None
        self.plants = import_plants()
        self._window = Ui_MainWindow()
        self._window.setupUi(self)

        self.rightColumn = self._window.rightColumn
        self.leftColumn = self._window.leftColumn
        self.connectionLabel = self._window.statusLabel

        self.waterBar = self._window.waterLevelBar
        self.moistureBar = self._window.moistureLevelBar
        self._set_ranges()
        self.set_water(0)
        self.set_moisture(0)

        self.plantsList = self._window.plantsList

        self.currentPlant = None
        self.set_disconnected()

        self.set_plants()
        self.plantsList.currentRowChanged.connect(self.set_selected_plant)
        self._window.refreshButton.clicked.connect(self.set_selected_plant)
        self._window.changeSettingsButton.clicked.connect(self.changeSettings)

        self.targetMoisture = 0
        self.waterInterval = 0

    def setConnection(self, plantConnection):
        self.conn = plantConnection

    def _set_ranges(self):
        self.waterBar.setMaximum(MAX_WATER_LEVEL)
        self.moistureBar.setMaximum(MAX_MOISTURE)

    def changeSettings(self):
        dialog = Settings(self.targetMoisture, self.waterInterval, self)
        dialog.settingsChanged.connect(self.setNewSettings)
        dialog.show()

    def setNewSettings(self, target_moisture, water_interval):
        index = self.plantsList.currentRow()
        plant = self.plants[index]
        print("set new settings", hex(plant.address), target_moisture, water_interval)
        self.set_disconnected()

        try:
            self.conn.write_moisture_wanted(plant.address, target_moisture)
            self.conn.write_water_interval(plant.address, water_interval)

            self.set_selected_plant(index)
            self.set_connected()
        except ResponseTimeout:
            print("Failed to set new settings")
            self.set_disconnected()


    def set_water(self, value):
        self.moistureBar.setValue(value)

    def set_moisture(self, value):
        self.moistureBar.setValue(value)

    def set_target_moisture(self, target_moisture):
        self._window.targetMoistureLabel.setText(str(target_moisture))
        self.targetMoisture = target_moisture

    def set_water_interval(self, water_interval):
        self._window.wateringMethodLabel.setText(str(water_interval))
        self.waterInterval = water_interval
    
    def set_status(self, water_level, moisture_level, target_moisture, water_interval):
        self.set_water(water_level)
        self.set_moisture(moisture_level)
        self.set_target_moisture(target_moisture)
        self.set_water_interval(water_interval)

    def set_connected(self):
        self.rightColumn.setEnabled(True)
        self.connectionLabel.setText("Connected")
        self.connectionLabel.setStyleSheet("QLabel {color : green;}")

    def set_disconnected(self):
        self.rightColumn.setEnabled(False)
        self.connectionLabel.setText("Disconnected")
        self.connectionLabel.setStyleSheet("QLabel {color : red;}")

    def set_plants(self):
        for plant in self.plants:
            self.plantsList.addItem(plant.name)

    def set_selected_plant(self, index):
        plant = self.plants[index]
        self.currentPlant = plant
        try:
            self.conn.ping(plant.address)
            moisture = self.conn.read_moisture(plant.address)
            water_level = self.conn.read_water_level(plant.address)
            target_moisture = self.conn.read_moisture_wanted(plant.address)
            water_interval = self.conn.read_water_interval(plant.address)
            self.set_status(water_level, moisture, target_moisture, water_interval)
            self.set_connected()
        except ResponseTimeout:
            print("Failed to ping device")
            self.set_disconnected()


class App(QApplication):
    def __init__(self, *args):
        super().__init__(*args)
        plants = [Plant("Plant 1", 0xa0), Plant("Plant 2", 0xb0)]

        available_ports = list([port.device for port in comports()])

        #self.portSelector = PortSelector(available_ports)
        #self.portSelector.show()

        #self.portSelector.portSelected.connect(self.set_port)
        #self.portSelector.canceled.connect(self.quit)

        self.main_win = MainWindow()
        self.set_port("/dev/ttyUSB0")

    def set_port(self, portName):
        #self.portSelector.close()
        connection = PlantConnection(0x01, Serial(portName, timeout = 2), 2)
        self.main_win.show()
        self.main_win.setConnection(connection)

if __name__ == "__main__":
    #app = App(argv)
    #app.exec_()

    port = Serial("/dev/ttyUSB0", timeout = 2)
    conn = PlantConnection(1, port, 2)
    print(conn.read_moisture_wanted(0xa0))
