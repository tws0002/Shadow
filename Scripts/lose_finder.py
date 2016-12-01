#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys
import glob
import shutil

import PySide.QtCore as QtCore
import PySide.QtGui as QtGui


class Frame(object):
    def __init__(self, frame, frame_file, render_pass):
        self.frame = frame
        self.render_pass = render_pass
        self.name = os.path.basename(frame_file)
        self.full_name = frame_file
        self.exists = False
        self.size = 0
        if os.path.exists(frame_file):
            self.exists = True
            self.size = os.path.getsize(frame_file)


class FrameModel(QtCore.QAbstractTableModel):
    def __init__(self, parent=None):
        super(FrameModel, self).__init__(parent)
        self.object_list = list()
        self.headers = ['Name', 'Frame', 'Size']

    def add(self, obj):
        self.object_list.append(obj)

    def get(self, index):
        return self.object_list[index.row()]

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
            h = self.headers[column]
            if 'Name' == h:
                return obj.name

            elif 'Frame' == h:
                return obj.frame

            elif 'Size' == h:
                return obj.size

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


class RenderPass(object):
    light_list = list()
    pass_list = list()

    def __init__(self, name, pass_file):
        self.name = name
        self.path = os.path.dirname(pass_file)
        self.ext = os.path.splitext(pass_file)[-1]
        self.frames = list()
        self.frame_num = 0
        self.current_num = 0
        self.lose_num = 0
        self.max_size = 0.001

        name_list = self.name.split('_')
        self.pass_type = name_list[-1]
        self.light_name = name_list[-2]

    def add_frame(self, frame):
        self.frames.append(frame)

    def check(self, start, end):
        self.frame_num = end - start + 1
        self.current_num = 0
        for i in range(start, end+1):
            frame = Frame(i, os.path.join(self.path, '%s.%04d%s' % (self.name, i, self.ext)), self)
            if frame.exists:
                self.current_num += 1
                self.max_size = max(self.max_size, frame.size)
            self.frames.append(frame)
        self.lose_num = self.frame_num - self.current_num

    def frame_at(self, frame):
        return os.path.join(self.path, '%s.%04d%s' % (self.name, frame, self.ext))


class RenderPassModel(QtCore.QAbstractTableModel):
    def __init__(self, parent=None):
        super(RenderPassModel, self).__init__(parent)
        self.object_list = list()
        self.headers = ['Name', 'Type', 'Light', 'Frames', 'Lost']

    def add(self, obj):
        self.object_list.append(obj)

    def get(self, index):
        return self.object_list[index.row()]

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
            h = self.headers[column]
            if 'Name' == h:
                return obj.name

            elif 'Type' == h:
                return obj.pass_type

            elif 'Light' == h:
                return obj.light_name

            elif 'Frames' == h:
                return obj.current_num

            elif 'Lost' == h:
                return obj.lose_num

        if role == QtCore.Qt.BackgroundRole:
            if obj.lose_num > 0:
                return QtGui.QBrush(QtGui.QColor(255, 255, 0, 100))

    def headerData(self, section, orientation, role):
        if role == QtCore.Qt.DisplayRole:
            if orientation == QtCore.Qt.Horizontal:
                return self.headers[section]
            else:
                # return "  %04d  " % section
                return "      "


class FramePortItem(QtGui.QGraphicsPathItem):
    Type = QtGui.QGraphicsItem.UserType + 10

    def __init__(self, frame, radius, parent=None):
        super(FramePortItem, self).__init__(parent)
        self.frame = frame
        self.radius = radius
        self.selected = False
        self.setToolTip(unicode(self.frame.frame))

    def update_path(self):
        path = QtGui.QPainterPath()
        path.addEllipse(0, 0, self.radius, self.radius)
        self.setPath(path)

    def paint(self, painter, option, widget):
        if self.selected:
            painter.setPen(QtGui.QPen(QtGui.QColor(255, 255, 0)))
            painter.setBrush(QtGui.QBrush(QtGui.QColor(255, 255, 0)))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(200, 100, 100)))
            painter.setBrush(QtGui.QBrush(QtGui.QColor(200, 100, 100)))
        painter.drawPath(self.path())

    def type(self):
        return self.Type


class FrameItem(QtGui.QGraphicsPathItem):
    Type = QtGui.QGraphicsItem.UserType + 11
    width = 10
    height = 100

    def __init__(self, frame, parent=None):
        super(FrameItem, self).__init__(parent)
        self.frame = frame
        self.size_height = float(self.frame.size) / self.frame.render_pass.max_size
        # self.port = QtGui.QPainterPath()
        self.port_r = self.width * 0.5
        self.port_x = self.port_r * 0.5
        self.port_y = self.height * (1 - self.size_height)
        self.selected = False
        self.setToolTip(unicode(self.frame.frame))

        self.port = FramePortItem(frame, self.port_r, self)

    def update_path(self):
        self.port_y = self.height * (1 - self.size_height)
        # self.port.addEllipse(self.port_x, self.port_y, self.port_r, self.port_r)
        self.port.setX(self.port_x)
        self.port.setY(self.port_y)
        self.port.update_path()

        path = QtGui.QPainterPath()
        path.addRect(0, 0, self.width, self.height + self.port_r)
        self.setPath(path)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtGui.QColor(220, 220, 255)))
        if self.selected:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(100, 100, 200, 100)))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(200, 200, 255, 100)))
        painter.drawPath(self.path())

        # painter.setPen(QtGui.QPen(QtGui.QColor(200, 100, 100)))
        # painter.setBrush(QtGui.QBrush(QtGui.QColor(200, 100, 100)))
        # painter.drawPath(self.port)

    def type(self):
        return self.Type

    def size_pos(self):
        out = self.scenePos() + QtCore.QPointF(self.port_x + self.port_x, self.port_y + self.port_x)
        return out


class FrameConnection(QtGui.QGraphicsPathItem):
    Type = QtGui.QGraphicsItem.UserType + 12

    def __init__(self, parent=None):
        super(FrameConnection, self).__init__(parent)
        self.frame_list = list()

        self.setZValue(-1)

    def add_frame(self, frame):
        self.frame_list.append(frame)

    def update_path(self):
        path = QtGui.QPainterPath()
        num = len(self.frame_list)
        for i in range(num):
            if i < (num - 1):
                path.moveTo(self.frame_list[i].size_pos())
                path.lineTo(self.frame_list[i+1].size_pos())
        self.setPath(path)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.darkYellow, 2))
        painter.drawPath(self.path())

    def type(self):
        return self.Type


class FrameGraphic(QtGui.QGraphicsScene):
    def __init__(self, parent=None):
        super(FrameGraphic, self).__init__(parent)
        self.rubber_band = None
        self.origin = QtCore.QPoint()
        self.start_pos = QtCore.QPoint()

    def item_at(self, position):
        items = self.items(QtCore.QRectF(position - QtCore.QPointF(2, 2), QtCore.QSizeF(4, 4)))
        for item in items:
            if item.type() == FramePortItem.Type:
                return item
        return None

    def un_select_all(self):
        for item in self.items():
            if item.type() == FramePortItem.Type:
                item.selected = False
                item.update()

    def mousePressEvent(self, event):
        super(FrameGraphic, self).mousePressEvent(event)
        self.start_pos = event.scenePos()
        self.origin = event.screenPos()
        if event.modifiers() == QtCore.Qt.ControlModifier:
            self.un_select_all()

        else:
            if event.button() == QtCore.Qt.LeftButton:
                if not self.rubber_band:
                    self.rubber_band = QtGui.QRubberBand(QtGui.QRubberBand.Rectangle, None)
                self.rubber_band.setGeometry(QtCore.QRect(self.origin, QtCore.QSize(0, 0)))
                self.rubber_band.show()

            if event.button() == QtCore.Qt.RightButton:
                pass

    def mouseMoveEvent(self, event):
        super(FrameGraphic, self).mouseMoveEvent(event)
        if self.rubber_band:
            self.rubber_band.setGeometry(QtCore.QRect(self.origin, event.screenPos()).normalized())

    def mouseReleaseEvent(self, event):
        super(FrameGraphic, self).mouseReleaseEvent(event)
        if event.button() == QtCore.Qt.LeftButton:
            if self.rubber_band:
                self.rubber_band.hide()

            items = self.items(QtCore.QRectF(self.start_pos, event.scenePos()))
            for item in items:
                if item.type() == FramePortItem.Type:
                    item.selected = not item.selected
                    item.update()

        if event.button() == QtCore.Qt.RightButton:
            item = self.item_at(event.scenePos())
            if item and item.type() == FramePortItem.Type:
                item.selected = not item.selected
                item.update()
            return True


class LoseFinder(QtGui.QWidget):
    def __init__(self, parent=None):
        super(LoseFinder, self).__init__(parent)
        self.folder = None
        self.render_pass = dict()
        self.pass_list = set()
        self.current_pass = None

        self.setWindowTitle("Lose Finder v1.0")
        self.resize(800, 600)

        self.main_layout = QtGui.QVBoxLayout()
        self.setLayout(self.main_layout)

        info = u'*** 更新说明：支持左键框选，右键点选, Ctrl+左键取消选择。 ***'
        self.main_layout.addWidget(QtGui.QLabel(info))

        self.toolbar = QtGui.QToolBar()
        self.main_layout.addWidget(self.toolbar)
        self.work_area = QtGui.QSplitter()
        self.work_area.setOrientation(QtCore.Qt.Vertical)
        self.main_layout.addWidget(self.work_area)

        self.pass_model = RenderPassModel()
        self.pass_filter_model = QtGui.QSortFilterProxyModel()
        self.pass_filter_model.setDynamicSortFilter(True)
        self.pass_filter_model.setSourceModel(self.pass_model)
        self.pass_table = QtGui.QTableView()
        self.work_area.addWidget(self.pass_table)
        self.pass_table.horizontalHeader().setStretchLastSection(True)
        self.pass_table.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.pass_table.setModel(self.pass_filter_model)
        self.pass_table.setSortingEnabled(True)
        self.pass_table.clicked.connect(self.show_pass)

        self.frame_view = QtGui.QTabWidget()
        self.work_area.addWidget(self.frame_view)
        self.frame_graph = FrameGraphic()
        self.frame_graph_view = QtGui.QGraphicsView(self.frame_graph)
        self.frame_view.addTab(self.frame_graph_view, u'图表')
        self.frame_model = FrameModel()
        self.frame_table = QtGui.QTableView()

        self.frame_table.horizontalHeader().setStretchLastSection(True)
        self.frame_table.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.frame_table.setModel(self.frame_model)
        self.frame_view.addTab(self.frame_table, u'表格')
        self.toolbar.addWidget(QtGui.QLabel(u' 开始帧: '))
        self.start = QtGui.QSpinBox()
        self.start.setMaximum(9999)
        self.start.setValue(101)
        self.toolbar.addWidget(self.start)
        self.toolbar.addWidget(QtGui.QLabel(u' 结束帧: '))
        self.end = QtGui.QSpinBox()
        self.end.setMaximum(9999)
        self.end.setValue(101)
        self.toolbar.addWidget(self.end)

        self.act_open = QtGui.QAction(u' 指定文件夹 ', self)
        self.act_open.triggered.connect(self.on_open)
        self.toolbar.addAction(self.act_open)

        self.path = QtGui.QLineEdit()
        self.toolbar.addWidget(self.path)

        self.act_find = QtGui.QAction(u' 分析 ', self)
        self.act_find.triggered.connect(self.do_find)
        self.toolbar.addAction(self.act_find)

        self.export_area = QtGui.QWidget()
        self.work_area.addWidget(self.export_area)
        self.export_layout = QtGui.QVBoxLayout()
        self.export_layout.setContentsMargins(0, 0, 0, 0)
        self.export_area.setLayout(self.export_layout)
        self.export_toolbar = QtGui.QToolBar()
        self.export_layout.addWidget(self.export_toolbar)

        self.act_clear_output = QtGui.QAction(u'清空', self)
        self.act_clear_output.triggered.connect(self.clear_output)
        self.export_toolbar.addAction(self.act_clear_output)
        self.act_print_frame = QtGui.QAction(u'打印结果', self)
        self.act_print_frame.triggered.connect(self.do_print_frame)
        self.export_toolbar.addAction(self.act_print_frame)
        self.act_fill_frame = QtGui.QAction(u'补帧（复制）', self)
        self.act_fill_frame.triggered.connect(self.do_fill_frame)
        self.export_toolbar.addAction(self.act_fill_frame)

        self.text_editor = QtGui.QTextEdit()
        self.text_editor.setWindowTitle(u"输出结果")
        self.export_layout.addWidget(self.text_editor)

    def clear(self):
        self.render_pass.clear()
        self.pass_model.clear()
        self.frame_model.clear()

    def on_open(self):
        self.folder = QtGui.QFileDialog.getExistingDirectory()
        self.path.setText(self.folder)

    def do_find(self):
        start = self.start.value()
        end = self.end.value()
        self.folder = os.path.normpath(self.path.text())
        if not self.folder:
            print(u'未指定目录.')
            return False
        file_list = glob.glob(os.path.join(self.folder, '*'))
        self.clear()
        if not file_list:
            return False

        for f in file_list:
            file_name = os.path.basename(f)
            pass_name = file_name.split('.')[0]
            if pass_name not in self.render_pass:
                self.render_pass[pass_name] = RenderPass(pass_name, f)
            # self.render_pass[pass_name].add_frame(f)

        for p in self.render_pass.values():
            p.check(start, end)
            self.pass_model.add(p)
        self.pass_model.reset()

        return True

    def show_frame_graph(self, frames):
        self.frame_graph.clear()
        FrameItem.height = self.frame_graph_view.height() * 0.8
        num = len(frames)
        self.frame_graph.setSceneRect(0, 0, FrameItem.width * num, FrameItem.height)
        frame_conn = FrameConnection()
        for i in range(num):
            d = FrameItem(frames[i])
            d.setX(i * FrameItem.width)
            d.update_path()
            self.frame_graph.addItem(d)
            frame_conn.add_frame(d)
        frame_conn.update_path()
        self.frame_graph.addItem(frame_conn)

    def show_frame_table(self, frames):
        self.frame_model.clear()
        for f in frames:
            self.frame_model.add(f)

    def show_pass(self, index):
        self.current_pass = self.pass_model.get(self.pass_filter_model.mapToSource(index))
        self.show_frame_graph(self.current_pass.frames)
        self.show_frame_table(self.current_pass.frames)

    def clear_output(self):
        self.text_editor.clear()

    def do_print_frame(self):
        if not self.text_editor.isVisible():
            self.text_editor.show()

        lose_list = list()
        for item in self.frame_graph.items():
            if item.type() == FramePortItem.Type:
                if item.selected:
                    lose_list.append(str(item.frame.frame))
        if self.current_pass:
            self.text_editor.append('%s: %s' % (self.current_pass.name, ','.join(lose_list)))
        else:
            self.text_editor.append(','.join(lose_list))

    def do_fill_frame(self):
        lose_list = list()
        for item in self.frame_graph.items():
            if item.type() == FramePortItem.Type:
                if item.selected:
                    lose_list.append(item.frame)

        def fill(frame, step):
            offset = 0
            while True:
                offset += step
                src = f.render_pass.frame_at(frame.frame - offset)
                if os.path.exists(src):
                    # print(src)
                    self.text_editor.append(u'修补 %s' % frame.full_name)
                    shutil.copy2(src, frame.full_name)
                    return True
                else:
                    if offset > 5 or offset < -5:
                        return False

        file_count = 0
        for f in lose_list:
            if fill(f, 1):
                file_count += 1
            elif fill(f, -1):
                file_count += 1

        self.text_editor.append(u'复制完成 %s 个文件.' % file_count)


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    w = LoseFinder()
    w.show()
    sys.exit(app.exec_())
