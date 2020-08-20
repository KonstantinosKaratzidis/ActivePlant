from PyQt5.QtWidgets import QWidget, QApplication
from generated.plant_list import Ui_PlantsList
import plant

class PlantsList(QWidget):
    def __init__(self, plants, *args):
        super().__init__(*args)
        self.list = Ui_PlantsList()
        self.list.setupUi(self)
        self.plants = plants

if __name__ == "__main__":
    import sys

    plants = set([plant.Plant("Plant 1", 0xa0), plant.Plant("Plant 2", 0xb0)])

    app = QApplication(sys.argv)
    lista = PlantsList(plants)
    lista.show()
    app.exec_()
