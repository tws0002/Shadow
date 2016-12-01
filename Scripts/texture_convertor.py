#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
  Author: kun.wang
  Create: 2013-10-31
'''
import os, sys
import json
import shutil
import subprocess

import PySide.QtCore as QtCore
import PySide.QtGui as QtGui

kSignal = QtCore.Signal
maketxcmd = "C:\\solidangle\\mtoadeploy\\2013\\bin\\maketx.exe"


def getDir():
    folder = unicode(QtGui.QFileDialog.getExistingDirectory())
    return folder

def getFile():
    file_name = unicode(QtGui.QFileDialog.getOpenFileName())
    return file_name

def getFiles():
    fs = []
    file_names = QtGui.QFileDialog.getOpenFileNames()
    for f in file_names:
        fs.append(unicode(f))
    return fs

def getSaveFile():
    file_name = unicode(QtGui.QFileDialog.getSaveFileName())
    return file_name


class TextureState():
    NotPub = 0
    Publish = 1
    Update = 2
    Wrong = 3
    StringList = ['Not Publish', 'Publish', 'Need Update', 'Wrong']


class Texture(object):
    def __init__(self, tex_file):
        self.__source = QtCore.QFileInfo(tex_file)
        self.__publish_dir = QtCore.QFileInfo()
        self.__publish = QtCore.QFileInfo()

        self.__state = TextureState.NotPub
        # Arnold option
        self.__arnold_static_option = '--oiio'
        self.__arnold_user_option = ''

    def name(self):
        return unicode(self.__source.fileName())

    def nameWithNewExt(self, ext):
        nl = self.name().split(".")
        nl[-1] = ext
        n = '.'.join(nl)
        return n

    def source(self):
        return unicode(self.__source.filePath())

    def publishAs(self, ext):
        return os.path.join(self.publishPath(), self.nameWithNewExt(ext)).replace("\\", "/")

    def publishSource(self):
        return os.path.join(self.publishPath(), self.name()).replace("\\", "/")

    def publishPath(self):
        return unicode(self.__publish_dir.filePath())

    def setPublishPath(self, folder):
        self.__publish_dir.setFile(folder)

    def state(self):
        return self.__state

    def stateAsStr(self):
        return TextureState.StringList[self.__state]

    def setOption(self, option):
        self.__arnold_user_option = option
        return True

    def option(self):
        return self.__arnold_user_option

    def makeArnoldCmd(self):
        cmd = maketxcmd
        cmd += ' -o "' + self.publishAs("tx") + '"'
        # Custom options
        cmd += ' ' + self.__arnold_static_option
        if self.__arnold_user_option:
            cmd += ' ' + self.__arnold_user_option
        cmd += ' "' + self.source() + '"'
        return cmd

    def publishAsArnold(self):
        cmd = self.makeArnoldCmd()
        subprocess.call(cmd, shell = True)

        shutil.copy(self.source(), self.publishSource())
        return True

    def analyse(self):
        self.__source.refresh()
        pubtx = QtCore.QFileInfo(self.publishAs('tx'))
        if pubtx.isFile():
            pubtx.refresh()
            if self.__source.lastModified() > pubtx.lastModified():
                self.__state = TextureState.Update
            else:
                self.__state = TextureState.Publish
        else:
            self.__state = TextureState.NotPub
        return True

    def data(self):
        tex = {}
        tex['name'] = self.name()
        tex['source'] = self.source()
        tex['publish'] = self.publishPath()
        tex['option'] = self.__arnold_user_option
        return tex

    def setData(self, data):
        self.__arnold_user_option = data.get('option', '')
        self.setPublishPath(data.get('publish', ''))
        return True


def makeTexture(data):
    tex = Texture(data['source'])
    tex.setData(data)
    return tex



class PublishTxTask(object):
    def __init__(self, iid, cmd):
        self.iid = iid
        self.cmd = cmd

    def setCmd(self, cmd):
        self.cmd = cmd

    def setCopy(self, src, dst):
        self.src = src
        self.dst = dst
        return True

    def doTask(self):
        subprocess.call(self.cmd, shell = True)
        shutil.copy(self.src, self.dst)
        return True


class PublishTxThread(QtCore.QThread):
    taskfinish = QtCore.Signal(int)
    def __init__(self):
        super(PublishTxThread, self).__init__()
        self.tasks = []

    def addTask(self, task):
        self.tasks.append(task)
        return True

    def addTasks(self, tasks):
        for t in tasks:
            self.addTask(t)

    def clearTask(self):
        del self.tasks[:]
        return True

    def run(self):
        for task in self.tasks:
            # subprocess.call(task.cmd, shell = True)
            task.doTask()
            self.taskfinish.emit(task.iid)
        self.clearTask()



class TextureTableMod(QtCore.QAbstractTableModel):
    def __init__(self, parent = None):
        super(TextureTableMod, self).__init__(parent)
        self.__project = ''
        self.__textures = []
        self.__headers = [u'State', u'Name', u'Option', u'Texture File', u'Publish Path']

    def addTexture(self, tex):
        self.__textures.append(tex)
        return True

    def addTextures(self, texs):
        for tex in texs:
            self.addTexture(tex)
        return True

    def removeTextures(self, texs):
        for tex in texs:
            self.__textures.remove(tex)
        return True

    def clear(self):
        del self.__textures[:]

    def rowCount(self, parent):
        return len(self.__textures)

    def columnCount(self, parent):
        return len(self.__headers)

    def flags(self, index):
        return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable# | QtCore.Qt.ItemIsEditable

    def data(self, index, role):
        row = index.row()
        column = index.column()
        tex = self.__textures[row]

        if role == QtCore.Qt.DisplayRole:
            h = self.__headers[column]
            if u'State' == h:
                return tex.stateAsStr()
            elif u'Name' == h:
                return tex.name()
            elif u'Option' == h:
                return tex.option()
            elif u'Texture File' == h:
                return tex.source()
            elif u'Publish Path' == h:
                return tex.publishPath()

        if role == QtCore.Qt.BackgroundRole:
            s = tex.state()
            if s == TextureState.NotPub:
                return QtGui.QBrush(QtGui.QColor(100, 100, 255, 200))
            if s == TextureState.Publish:
                return QtGui.QBrush(QtGui.QColor(0, 255, 0, 200))
            if s == TextureState.Wrong:
                return QtGui.QBrush(QtGui.QColor(255, 0, 0, 200))
            if s == TextureState.Update:
                return QtGui.QBrush(QtGui.QColor(255, 180, 0, 200))
            
    def headerData(self, section, orientation, role):
        if role == QtCore.Qt.DisplayRole:
            if orientation == QtCore.Qt.Horizontal:
                return self.__headers[section]
            else:
                return "  %04d  " % section
                # return "      "

    def getTexture(self, index):
        return self.__textures[index.row()]


    def getTextures(self, indexs):
        texs = []
        if not indexs:
            return texs
        for index in indexs:
            if index.column() == 0:
                tex_node = self.getTexture(index)
                texs.append(tex_node)
        return texs

    def allTextures(self):
        return self.__textures

class OptionSetting(QtGui.QDialog):
    def __init__(self, parent = None):
        super(OptionSetting, self).__init__(parent)
        self.resize(300, 50)
        self.__layout = QtGui.QGridLayout()
        self.setLayout(self.__layout)
        self.__layout.addWidget(QtGui.QLabel("Option:"), 0, 0, QtCore.Qt.AlignRight)

        self.option_list = QtGui.QComboBox()
        self.option_list.addItem('sRGB-Linear')
        self.option_list.addItem('Linear-sRGB')
        self.option_list.addItem('Custom')
        self.option_list.currentIndexChanged.connect(self.setPreset)
        self.__layout.addWidget(self.option_list, 0, 1, 1, 2)
        self.option = QtGui.QLineEdit('')
        self.__layout.addWidget(self.option, 1, 1, 1, 2)

        self.butOk = QtGui.QPushButton('OK')
        self.butOk.clicked.connect(self.accept)
        self.__layout.addWidget(self.butOk, 2, 1)
        self.butCancle = QtGui.QPushButton('Cancle')
        self.butCancle.clicked.connect(self.reject)
        self.__layout.addWidget(self.butCancle, 2, 2)

        self.setPreset()

    def setPreset(self):
        preset = str(self.option_list.currentText())
        if preset == 'sRGB-Linear':
            self.option.setText('--colorconvert sRGB linear')
        elif preset == 'Linear-sRGB':
            self.option.setText('--colorconvert linear sRGB')
        elif preset == 'Custom':
            self.option.setText('')

    def getOption(self):
        return unicode(self.option.text())


class TexturePublisher(QtGui.QWidget):
    def __init__(self, parent = None):
        super(TexturePublisher, self).__init__(parent)
        self.setProjectInfo('')
        self.resize(1024, 600)
        self.icon_size = 32

        self.__project_file = ''
        self.__project_path = ''
        self.__publish_thread = PublishTxThread()
        self.__publish_thread.taskfinish.connect(self.setFinish)
        self.__select_texs = []

        self.tx_list = TextureTableMod()

        self.tx_view = QtGui.QTableView()
        self.tx_view.setWordWrap(False)
        self.tx_view.horizontalHeader().setStretchLastSection(True)
        self.tx_view.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.tx_view.setModel(self.tx_list)
        
        ## Actions:
        self.actOpen = QtGui.QAction(u"Open", self)
        self.actOpen.setShortcut(QtGui.QKeySequence(QtGui.QKeySequence.Open))
        self.actOpen.triggered.connect(self.openProject)

        self.actSave = QtGui.QAction(u"Save", self)
        self.actSave.triggered.connect(self.doSave)
        self.actSave.setShortcut(QtGui.QKeySequence(QtGui.QKeySequence.Save))

        self.actSaveAs = QtGui.QAction(u"Save As", self)
        self.actSaveAs.triggered.connect(self.doSaveAs)

        self.actSetDefaltPubFolder = QtGui.QAction(u"Default Path", self)
        self.actSetDefaltPubFolder.triggered.connect(self.doSetDefalt)

        self.actAddTex = QtGui.QAction(u"Add Texture", self)
        self.actAddTex.triggered.connect(self.addTex)

        self.actSetPubFolder = QtGui.QAction(u"Set Path", self)
        self.actSetPubFolder.triggered.connect(self.doSetPubFolder)

        self.actSetOption = QtGui.QAction(u"Set Option", self)
        self.actSetOption.triggered.connect(self.setOption)

        self.actRemoveTex = QtGui.QAction(u"Remove", self)
        self.actRemoveTex.triggered.connect(self.removeTex)


        self.actAnalyse = QtGui.QAction(u"Analyse", self)
        self.actAnalyse.triggered.connect(self.doAnalyse)


        self.actPublish = QtGui.QAction(u"Publish", self)
        self.actPublish.triggered.connect(self.doPublish)

        self.actUpdate = QtGui.QAction(u"Update", self)
        self.actUpdate.triggered.connect(self.doUpdate)

        ## Toolbar
        self.__toolbar = QtGui.QToolBar()
        self.__toolbar.setOrientation(QtCore.Qt.Horizontal)

        self.__toolbar.addAction(self.actOpen)
        self.__toolbar.addAction(self.actSave)
        self.__toolbar.addAction(self.actSaveAs)

        self.__toolbar.addWidget(QtGui.QLabel(">>>"))
        self.__toolbar.addAction(self.actSetDefaltPubFolder)
        self.__toolbar.addAction(self.actAddTex)
        self.__toolbar.addAction(self.actSetPubFolder)
        self.__toolbar.addAction(self.actSetOption)
        self.__toolbar.addAction(self.actRemoveTex)

        self.__toolbar.addWidget(QtGui.QLabel(">>>"))
        self.__toolbar.addAction(self.actAnalyse)

        self.__toolbar.addWidget(QtGui.QLabel(">>>"))
        self.__toolbar.addAction(self.actPublish)
        self.__toolbar.addAction(self.actUpdate)

        self.__layout = QtGui.QVBoxLayout()
        self.__layout.setContentsMargins(0, 0, 0, 0)
        self.__layout.setSpacing(2)
        self.setLayout(self.__layout)
        self.__layout.addWidget(self.__toolbar)
        self.__layout.addWidget(self.tx_view)


    def setProjectInfo(self, proj):
        self.setWindowTitle('Pillars Texture : %s' % proj.replace("\\", "/"))

    def setOption(self):
        op = OptionSetting(self)
        if op.exec_() == QtGui.QDialog.Accepted:
            option = op.getOption()
            for t in self.tx_list.getTextures(self.tx_view.selectedIndexes()):
                t.setOption(option)
            self.tx_list.reset()
            return True
        else:
            return False

    def doSetDefalt(self):
        pdir = getDir()
        if pdir:
            self.__project_path = pdir
            self.setProjectInfo(self.__project_path)

    def openProject(self):
        pfile = getFile()
        if pfile:
            self.tx_list.clear()
            self.__project_file = pfile
            with open(self.__project_file, 'r') as f:
                # try:
                data = json.loads(f.read())
                self.__project_path = data.get('project', '')
                if self.__project_path:
                    self.setProjectInfo(self.__project_path)
                    for t in data.get('textures', []):
                        self.tx_list.addTexture(makeTexture(t))
                # except:
                #     pass
            self.doAnalyse()
        self.tx_view.resizeRowsToContents()

    def saveProject(self, file_name):
        with open(file_name, 'w') as f:
            data = {}
            data['project'] = self.__project_path
            data['textures'] = []
            for t in self.tx_list.allTextures():
                data['textures'].append(t.data())
            f.write(json.dumps(data, indent = 4))
        return True
        
    def doSave(self):
        if not self.__project_file:
            self.__project_file = getSaveFile()
        if self.__project_file:
            self.saveProject(self.__project_file)

    def doSaveAs(self):
        pfile = getSaveFile()
        if pfile:
            self.__project_file = pfile
            self.saveProject(self.__project_file)

    def addTex(self):
        tex_files = getFiles()
        for t in tex_files:
            tx = Texture(t)
            if self.__project_path:
                tx.setPublishPath(self.__project_path)
            self.tx_list.addTexture(tx)
        self.tx_list.reset()
        self.tx_view.resizeRowsToContents()
        return True

    def doSetPubFolder(self):
        pdir = getDir()
        if pdir:
            for tex in self.tx_list.getTextures(self.tx_view.selectedIndexes()):
                tex.setPublishPath(pdir)
            self.tx_list.reset()

    def removeTex(self):
        self.tx_list.removeTextures(self.tx_list.getTextures(self.tx_view.selectedIndexes()))
        self.tx_list.reset()

    def doPublish(self):
        self.__publish_thread.clearTask()
        del self.__select_texs[:]

        texs = self.tx_list.getTextures(self.tx_view.selectedIndexes())
        for i in xrange(len(texs)):
            tex = texs[i]
            self.__select_texs.append(tex)

            task = PublishTxTask(i, tex.makeArnoldCmd())
            task.setCopy(tex.source(), tex.publishSource())
            self.__publish_thread.addTask(task)

        self.__publish_thread.start()

        self.doAnalyse()
        self.doSave()

    def doAnalyse(self):
        for t in self.tx_list.allTextures():
            t.analyse()
        self.tx_list.reset()

    def doUpdate(self):
        for tex in self.tx_list.allTextures():
            if tex.state() == TextureState.Update:
                tex.publishAsArnold()
        self.doAnalyse()
        self.doSave()

    def setFinish(self, tx_id):
        self.__select_texs[tx_id].analyse()
        self.tx_list.reset()


def initEnv():
    with open('.setting', 'r') as f:
        data = json.loads(f.read())
        print data['arnold']

        
if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    mainwindow = TexturePublisher()
    mainwindow.show()
    sys.exit(app.exec_())