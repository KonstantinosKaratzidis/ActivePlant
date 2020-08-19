from sys import argv
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

class PlantList(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._layout = QGridLayout()

        # plants list widget
        self._plants = QListWidget()
        self._plants.addItem("Item 1")
        self._plants.addItem("Item 2")
        self._plants.addItem("Item 3")
        self._plants.addItem("Item 4")
        self._plants.addItem("Item 5")
        self._plants.addItem("Item 6")
        self._plants.addItem("Item 7")

        # controls
        controls = QWidget()

        add_button = QPushButton()
        add_button.setText("Add")
        remove_button = QPushButton()
        remove_button.setText("Remove")

        controls_layout = QGridLayout()
        controls_layout.addWidget(add_button)
        controls_layout.addWidget(remove_button)
        controls.setLayout(controls_layout)

        self._layout.addWidget(self._plants)
        self._layout.addWidget(controls)

        self.setLayout(self._layout)

class PlantInfo(QWidget):
    def __init__(self, *args):
        super().__init__(*args)
        self._layout = QGridLayout()

        info_box = QWidget(self)
        info_box_layout = QGridLayout()
        info_box_layout.setHorizontalSpacing(30)

        info_box_layout.addWidget(QLabel("Soil Moisture"), 0, 0)
        info_box_layout.addWidget(QLabel("70%"), 0, 1)

        info_box_layout.addWidget(QLabel("Target Moisture Level"), 1, 0)
        info_box_layout.addWidget(QLabel("80 %"), 1, 1)

        info_box_layout.addWidget(QLabel("Watering plan"), 2, 0)
        info_box_layout.addWidget(QLabel("Periodic: 7 days, 2 hours"), 2, 1)

        info_box_layout.addWidget(QLabel("Tank Water Level"), 3, 0)
        info_box_layout.addWidget(QLabel("25 %"), 3, 1)
        info_box.setLayout(info_box_layout)

        self._layout.addWidget(QLabel("Plant Status"), 0, 0)
        self._layout.addWidget(info_box, 1, 0)

        self._layout.addWidget(QLabel("Placeholder Plant Water level icon"))

        settings_button = QPushButton()
        settings_button.setText("Change settings")
        self._layout.addWidget(settings_button)

        self.setLayout(self._layout)

class PlantWindow(QWidget):
    def __init__(self, *args):
        super().__init__(*args)
        col_layout = QGridLayout()
        plant_info = PlantInfo(self)

class PlantSettings(QDialog):
    def __init__(self, *args):
        super().__init__(*args)
        self.setModal(True)


        form = QWidget(self)
        form_layout = QFormLayout(form)
        form_layout.addRow(QLabel("Target Moisture:"), QSlider(Qt.Horizontal))
        form_layout.addRow(QLabel("Period:"), QLineEdit())

        settings_layout = QGridLayout(self)
        settings_layout.addWidget(form, 0, 0, 1, 2)

        settings_layout.addWidget(QPushButton("Apply"))
        settings_layout.addWidget(QPushButton("Cancel"))

        self.setLayout(settings_layout)
        
class GuiWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setGeometry(0, 0, 600, 400)
        self.setWindowTitle("ActivePlant")

        layout = QGridLayout()
        layout.addWidget(PlantList(self), 0, 0)
        layout.addWidget(PlantWindow(), 0, 1)
        layout.setColumnStretch(1, 1)
        self.setLayout(layout)
        self.set_center()
        PlantSettings(self).show()

    def set_center(self):
        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().center()
        qtRectangle.moveCenter(centerPoint)
        self.move(qtRectangle.topLeft())

def main():
    app = QApplication(argv)
    win = GuiWindow()
    win.show()
    app.exec_()

if __name__ == "__main__":
    main()
