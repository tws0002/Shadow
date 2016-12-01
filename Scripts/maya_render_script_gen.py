#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import glob
import datetime

import PySide.QtCore as QtCore
import PySide.QtGui as QtGui


class MayaFileListItem(QtGui.QListWidgetItem):
    def __init__(self, file_name, parent=None):
        super(MayaFileListItem, self).__init__(parent)
        self.name = os.path.basename(file_name)
        self.full_name = file_name
        self.setText(self.name)


class MayaRenderScriptGen(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MayaRenderScriptGen, self).__init__(parent)

        self.setWindowTitle("Maya Render Script Gen v1.0")
        self.resize(800, 400)

        self.main_layout = QtGui.QVBoxLayout()
        self.setLayout(self.main_layout)

        self.toolbar = QtGui.QToolBar()
        self.main_layout.addWidget(self.toolbar)

        self.act_set_folder = QtGui.QAction(u' 指定文件夹 ', self)
        self.act_set_folder.triggered.connect(self.set_folder)
        self.toolbar.addAction(self.act_set_folder)
        self.path = QtGui.QLineEdit()
        self.path.textChanged.connect(self.load_maya_files)
        self.toolbar.addWidget(self.path)
        self.act_update = QtGui.QAction(u'更新', self)
        self.act_update.triggered.connect(self.load_maya_files)
        self.toolbar.addAction(self.act_update)

        self.maya_file_list = QtGui.QListWidget()
        self.maya_file_list.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.main_layout.addWidget(self.maya_file_list)

        self.option_layout = QtGui.QGridLayout()
        self.main_layout.addLayout(self.option_layout)

        self.start = QtGui.QLineEdit(u'暂不可用')
        self.end = QtGui.QLineEdit(u'暂不可用')
        self.step = QtGui.QLineEdit(u'暂不可用')
        self.start.setDisabled(True)
        self.end.setDisabled(True)
        self.step.setDisabled(True)
        self.frame = QtGui.QLineEdit()
        self.render_layer = QtGui.QLineEdit()
        self.maya_version = QtGui.QComboBox()
        self.maya_version.addItems(['2011', '2012', '2013', '2014', '2015', '2016'])
        self.maya_version.setCurrentIndex(4)
        self.maya_version.currentIndexChanged.connect(self.set_maya_path)
        self.maya_path = QtGui.QLineEdit()
        # self.output_path = QtGui.QLineEdit()

        self.option_layout.addWidget(QtGui.QLabel(u'开始帧：'), 0, 0, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.start, 0, 1)
        self.option_layout.addWidget(QtGui.QLabel(u'结束帧：'), 0, 2, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.end, 0, 3)
        self.option_layout.addWidget(QtGui.QLabel(u'步进：'), 0, 4, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.step, 0, 5)
        self.option_layout.addWidget(QtGui.QLabel(u'单帧(101,102)：'), 1, 0, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.frame, 1, 1, 1, 5)
        self.option_layout.addWidget(QtGui.QLabel(u'渲染层(key,fill)：'), 2, 0, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.render_layer, 2, 1, 1, 5)
        self.option_layout.addWidget(QtGui.QLabel(u'Maya：'), 3, 0, QtCore.Qt.AlignRight)
        self.option_layout.addWidget(self.maya_version, 3, 1)
        self.option_layout.addWidget(self.maya_path, 3, 2, 1, 4)
        # self.option_layout.addWidget(QtGui.QLabel(u'输出路径：'), 4, 0, QtCore.Qt.AlignRight)
        # self.option_layout.addWidget(self.output_path, 4, 1, 1, 5)

        self.export_layout = QtGui.QHBoxLayout()
        self.main_layout.addLayout(self.export_layout)
        self.export_layout.addStretch()
        self.bt_export_one = QtGui.QPushButton(u'生成单bat')
        self.bt_export_one.clicked.connect(self.export_one_file)
        self.export_layout.addWidget(self.bt_export_one)
        self.bt_export_separate = QtGui.QPushButton(u'生成多bat')
        self.bt_export_separate.clicked.connect(self.export_separate_file)
        self.export_layout.addWidget(self.bt_export_separate)

        self.set_maya_path()

    def set_folder(self):
        folder = QtGui.QFileDialog.getExistingDirectory()
        self.path.setText(folder)

    def load_maya_files(self):
        path = self.path.text()
        if not os.path.exists(path):
            return False
        self.maya_file_list.clear()
        ma_list = glob.glob(os.path.join(path, '*.ma'))
        for ma in ma_list:
            self.maya_file_list.addItem(MayaFileListItem(ma))
        mb_list = glob.glob(os.path.join(path, '*.mb'))
        for mb in mb_list:
            self.maya_file_list.addItem(MayaFileListItem(mb))
        return True

    def set_maya_path(self):
        self.maya_path.setText('C:\\Program Files\\Autodesk\\Maya%s\\bin' % self.maya_version.currentText())

    def export_one_file(self):
        self.export_bat(True)

    def export_separate_file(self):
        self.export_bat(False)

    def export_bat(self, one=True):
        items = self.maya_file_list.selectedItems()
        path = self.path.text()
        # start = self.start.text()
        # end = self.end.text()
        # step = self.step.text()
        frame = self.frame.text()
        layer = self.render_layer.text().replace(' ', '')
        maya_cmd = os.path.join(self.maya_path.text(), 'render.exe')
        # out_path = self.output_path.text()
        file_count = 0
        cmd_list = list()
        for item in items:
            name = item.name.split('.')[0]
            if frame:
                for l in layer.split(','):
                    for f in frame.split(','):
                        if l:
                            bat_name = os.path.join(path, '%s_%s_%s.bat' % (name, l, f))
                            out = '\"%s\" -r arnold -rl %s -s %s -e %s -proj %s %s' \
                                  % (maya_cmd, l, f, f, path, item.full_name)
                        else:
                            bat_name = os.path.join(path, '%s_%s.bat' % (name, f))
                            out = '\"%s\" -r arnold -s %s -e %s -proj %s %s' \
                                  % (maya_cmd, f, f, path, item.full_name)

                        cmd_list.append([bat_name, out])

        if one:
            bat_file = os.path.join(path, 'batch_%s.bat' % datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S_%f'))
            with open(bat_file, 'w') as f_out:
                for cmd in cmd_list:
                    f_out.write(cmd[1])
                    f_out.write('\n')
                    f_out.write('\n')
                f_out.write('echo & pause exit\n')
                file_count = 1
        else:
            for cmd in cmd_list:
                with open(cmd[0], 'w') as f_out:
                    f_out.write(cmd[1])
                    f_out.write('\n')
                    f_out.write('echo & pause exit\n')
                    file_count += 1

        QtGui.QMessageBox.about(None, u'完成', u'输出 %s 个文件.' % file_count)


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    w = MayaRenderScriptGen()
    w.show()
    sys.exit(app.exec_())
