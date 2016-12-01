#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
  Author: kun.wang
  Create: 2014-09-23
"""
import sys

from PySide import QtCore
from PySide import QtGui


class PathReplacer(QtGui.QWidget):
    def __init__(self, parent=None):
        super(PathReplacer, self).__init__(parent)
        self.setWindowTitle("Path Replacer")
        self.resize(1000, 30)
        self.__layout = QtGui.QGridLayout()
        self.setLayout(self.__layout)

        self.win_prefix = QtGui.QLineEdit('P:/')
        self.linux_prefix = QtGui.QLineEdit('/Pillars/')

        self.__layout.addWidget(QtGui.QLabel('Win :'), 0, 1, 1, 1, QtCore.Qt.AlignRight)
        self.__layout.addWidget(self.win_prefix, 0, 2, 1, 1)
        self.__layout.addWidget(QtGui.QLabel('Linux :'), 0, 3, 1, 1, QtCore.Qt.AlignRight)
        self.__layout.addWidget(self.linux_prefix, 0, 4, 1, 1)

        self.winPath = QtGui.QLineEdit()
        self.winPath.textChanged.connect(self.win_changed)
        self.linuxPath = QtGui.QLineEdit()
        self.linuxPath.textChanged.connect(self.linux_changed)

        self.__layout.addWidget(QtGui.QLabel("Win Path : "), 1, 0, QtCore.Qt.AlignRight)
        self.__layout.addWidget(self.winPath, 1, 1, 1, 4)
        self.__layout.addWidget(QtGui.QLabel("Linux Path : "), 2, 0, QtCore.Qt.AlignRight)
        self.__layout.addWidget(self.linuxPath, 2, 1, 1, 4)

    def win_changed(self, s):
        self.winPath.setText(s.replace("\\", "/"))
        self.linuxPath.setText(s.replace(self.win_prefix.text(), self.linux_prefix.text()))
        return True

    def linux_changed(self, s):
        self.winPath.setText(s.replace(self.linux_prefix.text(), self.win_prefix.text()))
        return True


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    pr = PathReplacer()
    pr.show()
    sys.exit(app.exec_())
