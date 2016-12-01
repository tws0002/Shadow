#!/usr/bin/python
# -*- coding: utf-8 -*-
"""

"""

import PySide.QtCore as QtCore
import PySide.QtGui as QtGui
from arnold_env import *
from arnold import *


def ai_begin():
    AiBegin()
    AiLoadPlugins(ARNOLD_PLUGIN_PATH)


def ai_end():
    AiEnd()


class Texture(object):
    def __init__(self):
        self.ass_file = ''
        self.node_name = ''
        self.node_type = ''
        self.file_name = ''

        self.exists = False

    def check(self):
        if os.path.isfile(self.file_name):
            self.exists = True


def get_file_textures(ass_file):
    ai_begin()
    AiASSLoad(ass_file)

    tex_list = []

    it = AiUniverseGetNodeIterator(AI_NODE_SHADER)
    while not AiNodeIteratorFinished(it):
        node = AiNodeIteratorGetNext(it)
        if AiNodeIs(node, "image"):
            tex = Texture()
            tex.ass_file = ass_file
            tex.node_name = AiNodeGetStr(node, "name")
            tex.file_name = AiNodeGetStr(node, "filename")
            tex.node_type = 'image'
            tex.check()
            tex_list.append(tex)
        elif AiNodeIs(node, "MayaFile"):
            tex = Texture()
            tex.ass_file = ass_file
            tex.node_name = AiNodeGetStr(node, "name")
            tex.file_name = AiNodeGetStr(node, "filename")
            tex.node_type = 'MayaFile'
            tex.check()
            tex_list.append(tex)
    AiNodeIteratorDestroy(it)

    ai_end()

    return tex_list


def change_file_textures(ass_file, textures):
    ai_begin()
    AiASSLoad(ass_file)

    for tex in textures:
        node = AiNodeLookUpByName(tex.node_name)
        AiNodeSetStr(node, "filename", tex.file_name)

    AiASSWrite(ass_file)
    ai_end()


class TextureModel(QtCore.QAbstractTableModel):
    def __init__(self, parent=None):
        super(TextureModel, self).__init__(parent)
        self.object_list = list()
        self.headers = [u'节点名', u'类型', u'文件']

    def add(self, obj):
        self.object_list.append(obj)

    def get(self, index):
        return self.object_list[index.row()]

    def get_many(self, indexes):
        obj_list = []
        if not indexes:
            return obj_list
        for index in indexes:
            if index.column() == 0:
                obj = self.get(index)
                obj_list.append(obj)
        return obj_list

    def clear(self):
        del self.object_list[:]
        self.reset()

    def rowCount(self, parent):
        return len(self.object_list)

    def columnCount(self, parent):
        return len(self.headers)

    def flags(self, index):
        return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable

    def data(self, index, role):
        row = index.row()
        column = index.column()
        obj = self.object_list[row]

        if role == QtCore.Qt.DisplayRole:
            # h = self.headers[column]
            if 0 == column:
                return obj.node_name

            elif 1 == column:
                return obj.node_type

            elif 2 == column:
                return obj.file_name

        if role == QtCore.Qt.BackgroundRole:
            if not obj.exists:
                return QtGui.QBrush(QtGui.QColor(255, 0, 0, 100))

    def headerData(self, section, orientation, role):
        if role == QtCore.Qt.DisplayRole:
            if orientation == QtCore.Qt.Horizontal:
                return self.headers[section]
            else:
                # return "  %04d  " % section
                return "      "


class TexReplacer(QtGui.QWidget):
    def __init__(self, parent=None):
        super(TexReplacer, self).__init__(parent)
        self.title = 'Texture Replacer'
        self.setWindowTitle(self.title)
        self.resize(800, 600)

        self.ass_file = ''

        self.act_load_ass = QtGui.QAction(u"打开Ass", self)
        self.act_load_ass.triggered.connect(self.load_ass)

        self.act_change_path = QtGui.QAction(u"设置路径", self)
        self.act_change_path.triggered.connect(self.set_path)

        self.main_layout = QtGui.QVBoxLayout()
        self.setLayout(self.main_layout)

        self.toolbar = QtGui.QToolBar()
        self.main_layout.addWidget(self.toolbar)
        self.toolbar.addAction(self.act_load_ass)
        self.toolbar.addAction(self.act_change_path)

        self.ass_file_info = QtGui.QLabel()
        self.main_layout.addWidget(self.ass_file_info)

        self.tex_model = TextureModel()
        self.tex_table = QtGui.QTableView()

        self.tex_table.horizontalHeader().setStretchLastSection(True)
        self.tex_table.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.tex_table.setModel(self.tex_model)
        self.main_layout.addWidget(self.tex_table)

    def load_ass(self):
        self.tex_model.clear()
        self.ass_file = QtGui.QFileDialog.getOpenFileName(self,
                                                          "Select one or more .ass files", "", "Ass (*.ass)")[0]
        if not self.ass_file:
            self.ass_file_info.setText("not load ass file.")
            return False

        self.ass_file_info.setText(self.ass_file)
        self.tex_model.object_list = get_file_textures(self.ass_file)
        self.tex_model.reset()

        return True

    def set_path(self):
        folder = unicode(QtGui.QFileDialog.getExistingDirectory(self, "Select folder to set to."))
        if not folder:
            return False
        
        tex_list = self.tex_model.get_many(self.tex_table.selectedIndexes())
        for tex in tex_list:
            new_file = os.path.join(folder, os.path.basename(tex.file_name))
            tex.file_name = new_file.replace("\\", "/")

        change_file_textures(self.ass_file, tex_list)
        for tex in self.tex_model.object_list:
            tex.check()
        self.tex_model.reset()
        return True


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    arnold = TexReplacer()
    arnold.show()
    sys.exit(app.exec_())
