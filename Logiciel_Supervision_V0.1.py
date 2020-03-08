"""
Auteur: xTinox
Titre: Logiciel de supervision pour bouton connecté au serveur MQTT
Version: 0.1
Date: 08/03/2020
Commentaire(s): Essai SUBSCRIBE et de l'interface en QT (fonctionnel)

ATTENTION : Avant de "Run" le programme veuillez installer les bibliothèques
            suivantes à l'aide de l'invide de commande
            (utilisé "pip install LE_NOM_DE_LA_BIBLIOTHÈQUE"):
            - PyQt5
            - paho-mqtt
"""
from PyQt5 import QtCore, QtGui, QtWidgets
import paho.mqtt.client as mqtt

client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    client.subscribe("le_nom_du_topic")

client.on_connect = on_connect

client.connect("test.mosquitto.org", 1883, 60)
client.loop_start()

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(60, 60)
        self.widget = QtWidgets.QWidget(Dialog)
        self.widget.setGeometry(QtCore.QRect(0, 0, 50, 50))
        self.widget.setObjectName("widget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.widget)
        self.verticalLayout.setContentsMargins(10, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtWidgets.QLabel(self.widget)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.frame = QtWidgets.QFrame(self.widget)
        self.frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout.addWidget(self.frame)
        self.frame.raise_()

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

        def on_message(client, userdata, msg):
            message=str(msg.payload)
            liste=[]
            for i in range (2,len(message)-1):
                liste.append(message[i])
            chaine="".join(liste)
            print(chaine)
            if chaine=="0":
                self.frame.setStyleSheet("background-color: rgb(0, 0, 0);")
            elif chaine=="1":
                self.frame.setStyleSheet("background-color: rgb(0, 0, 255);")
            elif chaine=="2":
                self.frame.setStyleSheet("background-color: rgb(0, 255, 0);")
            elif chaine=="3":
                self.frame.setStyleSheet("background-color: rgb(255, 0, 0);")

        client.on_message = on_message

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.label.setText(_translate("Dialog", "POSTE 1"))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())
