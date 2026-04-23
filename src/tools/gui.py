#!/usr/bin/env python3
"""
SDF2 Studio - Live GUI for the SDF2 scripting language.

Left panel : script editor with syntax highlighting
Right panel: live 3D STL preview (vedo/VTK embedded in Qt)
Bottom bar : Export STL, Binary/Coalesce checkboxes, status

Usage:
    python gui.py [script.sdf]
"""

import os
import re
import subprocess
import sys
import tempfile

from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QColor, QFont, QKeySequence, QSyntaxHighlighter, QTextCharFormat
from PyQt5.QtWidgets import (
    QApplication, QCheckBox, QFileDialog, QHBoxLayout, QLabel,
    QMainWindow, QPushButton, QShortcut, QSplitter, QTextEdit, QVBoxLayout,
    QWidget,
)

from vtkmodules.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
from vedo import Mesh, Plotter, Text2D


# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

_HERE = os.path.dirname(os.path.abspath(__file__))
_ROOT = os.path.abspath(os.path.join(_HERE, "..", ".."))

def _find_binary() -> str:
    for build in ("release", "debug"):
        p = os.path.join(_ROOT, "build", build, "sdf2_script")
        if os.path.exists(p):
            return p
    return os.path.join(_ROOT, "build", "debug", "sdf2_script")

SCRIPT_BIN = _find_binary()

_TEMP_DIR = tempfile.gettempdir()
_TEMP_SDF = os.path.join(_TEMP_DIR, "sdf2_studio_script.sdf")
_TEMP_STL = os.path.join(_TEMP_DIR, "sdf2_studio_preview.stl")

DEFAULT_SCRIPT = """\
# SDF2 Script  –  assign your final form to 'out'
# Functions: sphere, box, cylinder, torus, circprism, inscprism
#            union, intersection, difference
#            translate, rotate, mirror, pattern
# Axes: X  Y  Z

out = sphere(10)
"""


# ---------------------------------------------------------------------------
# Syntax highlighter
# ---------------------------------------------------------------------------

_KEYWORDS = (
    "sphere", "box", "cylinder", "circprism", "inscprism", "torus",
    "union", "intersection", "difference",
    "translate", "rotate", "mirror", "pattern",
    "out", "X", "Y", "Z",
)
_KW_RE  = re.compile(r"\b(" + "|".join(_KEYWORDS) + r")\b")
_NUM_RE = re.compile(r"\b\d+(\.\d+)?\b")
_CMT_RE = re.compile(r"#.*$")


class _Highlighter(QSyntaxHighlighter):
    def __init__(self, doc):
        super().__init__(doc)
        self._kw  = self._fmt("#569cd6")
        self._num = self._fmt("#b5cea8")
        self._cmt = self._fmt("#6a9955")

    @staticmethod
    def _fmt(hex_color: str) -> QTextCharFormat:
        f = QTextCharFormat()
        f.setForeground(QColor(hex_color))
        return f

    def highlightBlock(self, text: str):
        visible = text
        m = _CMT_RE.search(text)
        if m:
            self.setFormat(m.start(), len(text) - m.start(), self._cmt)
            visible = text[: m.start()]
        for m in _KW_RE.finditer(visible):
            self.setFormat(m.start(), m.end() - m.start(), self._kw)
        for m in _NUM_RE.finditer(visible):
            self.setFormat(m.start(), m.end() - m.start(), self._num)


# ---------------------------------------------------------------------------
# 3-D viewer widget (vedo / VTK embedded in Qt)
# ---------------------------------------------------------------------------

class STLViewer(QWidget):
    _BG   = "#1e1e1e"
    _MESH = "#4a9eff"

    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)

        self._vtk_widget = QVTKRenderWindowInteractor(self)
        layout.addWidget(self._vtk_widget)

        self._plt = Plotter(qt_widget=self._vtk_widget, bg=self._BG, axes=1)
        self._plt.show(interactive=False)
        self._txt = None

    def _remove_text(self):
        if self._txt is not None:
            self._plt.remove(self._txt, render=False)
            self._txt = None

    def show_placeholder(self, msg: str, color: str = "#555555"):
        self._remove_text()
        self._plt.clear()
        self._txt = Text2D(msg, pos="center", c=color, s=1.1)
        self._plt += self._txt
        self._plt.render()

    def load_stl(self, path: str) -> bool:
        try:
            m = Mesh(path)
            if m.npoints == 0:
                return False
            m.color(self._MESH).alpha(0.9)
            self._remove_text()
            self._plt.clear()
            self._plt.show(m, interactive=False, resetcam=True)
            return True
        except Exception:
            return False


# ---------------------------------------------------------------------------
# Main window
# ---------------------------------------------------------------------------

_STYLE = """
QMainWindow, QWidget       { background: #1e1e1e; color: #d4d4d4; }
QTextEdit {
    background: #252526; color: #d4d4d4;
    border: 1px solid #3c3c3c; border-radius: 4px;
    selection-background-color: #264f78;
}
QPushButton {
    background: #0e639c; color: #ffffff;
    border: none; border-radius: 4px;
    padding: 5px 16px; font-size: 13px;
}
QPushButton:hover   { background: #1177bb; }
QPushButton:pressed { background: #0a4f7a; }
QPushButton:disabled{ background: #3c3c3c; color: #666666; }
QCheckBox           { color: #d4d4d4; spacing: 6px; font-size: 13px; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border: 1px solid #555555; border-radius: 3px;
    background: #3c3c3c;
}
QCheckBox::indicator:checked { background: #0e639c; border-color: #0e639c; }
QSplitter::handle   { background: #3c3c3c; width: 2px; }
"""


class MainWindow(QMainWindow):
    def __init__(self, script_path: str | None = None):
        super().__init__()
        self.setWindowTitle("SDF2 Studio")
        self.resize(1280, 760)

        self._script_path  = script_path
        self._build_proc   = None
        self._export_proc  = None
        self._export_path  = ""

        self._debounce = QTimer(singleShot=True, interval=600)
        self._debounce.timeout.connect(self._trigger_build)

        self._poll = QTimer(interval=80)
        self._poll.timeout.connect(self._check_build)

        self._export_poll = QTimer(interval=80)
        self._export_poll.timeout.connect(self._check_export)

        self._build_ui()
        self.setStyleSheet(_STYLE)

        if script_path and os.path.exists(script_path):
            with open(script_path) as f:
                self._editor.setPlainText(f.read())
        else:
            self._editor.setPlainText(DEFAULT_SCRIPT)

        QTimer.singleShot(200, self._trigger_build)

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _build_ui(self):
        root = QWidget()
        self.setCentralWidget(root)
        vbox = QVBoxLayout(root)
        vbox.setContentsMargins(8, 8, 8, 8)
        vbox.setSpacing(6)

        splitter = QSplitter(Qt.Horizontal)
        vbox.addWidget(splitter, stretch=1)

        # ---- editor (left) ----
        self._editor = QTextEdit()
        self._editor.setFont(QFont("Monospace", 11))
        self._editor.setLineWrapMode(QTextEdit.NoWrap)
        self._editor.textChanged.connect(self._on_text_changed)
        _Highlighter(self._editor.document())
        splitter.addWidget(self._editor)

        # ---- viewer (right) ----
        self._viewer = STLViewer(parent=self)
        splitter.addWidget(self._viewer)
        splitter.setSizes([420, 820])

        # ---- bottom bar ----
        bar = QHBoxLayout()
        vbox.addLayout(bar)

        self._export_btn = QPushButton("Export STL")
        self._export_btn.setFixedHeight(32)
        self._export_btn.setToolTip("Save to a chosen file using the current Binary/Coalesce settings")
        self._export_btn.clicked.connect(self._export_stl)
        bar.addWidget(self._export_btn)

        bar.addSpacing(14)

        self._binary_cb = QCheckBox("Binary")
        self._binary_cb.setChecked(True)
        self._binary_cb.setToolTip("Write binary STL (smaller, faster) instead of ASCII")
        self._binary_cb.stateChanged.connect(self._on_option_changed)
        bar.addWidget(self._binary_cb)

        self._coalesce_cb = QCheckBox("Coalesce")
        self._coalesce_cb.setChecked(False)
        self._coalesce_cb.setToolTip("Merge coplanar triangles (smaller mesh, slower build)")
        self._coalesce_cb.stateChanged.connect(self._on_option_changed)
        bar.addWidget(self._coalesce_cb)

        bar.addStretch()

        self._status_lbl = QLabel("Ready")
        self._status_lbl.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        self._status_lbl.setMinimumWidth(340)
        bar.addWidget(self._status_lbl)

        # Ctrl+Enter / Cmd+Enter → force rebuild
        shortcut = QShortcut(QKeySequence("Ctrl+Return"), self)
        shortcut.activated.connect(self._trigger_build)

    # ------------------------------------------------------------------
    # Live rebuild
    # ------------------------------------------------------------------

    def _on_text_changed(self):
        self._debounce.start()

    def _on_option_changed(self):
        if not self._debounce.isActive():
            self._trigger_build()

    def _trigger_build(self):
        self._debounce.stop()

        if self._build_proc and self._build_proc.poll() is None:
            self._build_proc.kill()
            self._build_proc.wait()

        if not os.path.exists(SCRIPT_BIN):
            self._set_status(f"Binary not found: {SCRIPT_BIN}", error=True)
            return

        with open(_TEMP_SDF, "w") as f:
            f.write(self._editor.toPlainText())

        # Always use binary for the preview (faster parse); honour coalesce.
        cmd = [SCRIPT_BIN, "-s", _TEMP_SDF, "-o", _TEMP_STL, "-b"]
        if self._coalesce_cb.isChecked():
            cmd.append("-c")

        self._set_status("Building…", building=True)

        self._build_proc = subprocess.Popen(
            cmd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        self._poll.start()

    def _check_build(self):
        if self._build_proc is None:
            self._poll.stop()
            return
        ret = self._build_proc.poll()
        if ret is None:
            return
        self._poll.stop()

        _, stderr = self._build_proc.communicate()
        if ret == 0:
            ok = self._viewer.load_stl(_TEMP_STL)
            self._set_status("Ready" if ok else "STL parse failed", error=(not ok))
            if not ok:
                self._viewer.show_placeholder("STL parse failed", color="#ff6b6b")
        else:
            msg = stderr.decode("utf-8", errors="replace").strip().removeprefix("Error: ")
            self._set_status(msg, error=True)
            self._viewer.show_placeholder("Build error", color="#ff6b6b")

    # ------------------------------------------------------------------
    # Export
    # ------------------------------------------------------------------

    def _export_stl(self):
        default = os.path.expanduser("~/out/export.stl")
        path, _ = QFileDialog.getSaveFileName(
            self, "Export STL", default, "STL Files (*.stl);;All Files (*)"
        )
        if not path:
            return

        with open(_TEMP_SDF, "w") as f:
            f.write(self._editor.toPlainText())

        cmd = [SCRIPT_BIN, "-s", _TEMP_SDF, "-o", path]
        if self._binary_cb.isChecked():
            cmd.append("-b")
        if self._coalesce_cb.isChecked():
            cmd.append("-c")

        self._export_path = path
        self._export_btn.setEnabled(False)
        self._set_status(f"Exporting {os.path.basename(path)}…", building=True)

        self._export_proc = subprocess.Popen(
            cmd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        self._export_poll.start()

    def _check_export(self):
        ret = self._export_proc.poll()
        if ret is None:
            return
        self._export_poll.stop()
        self._export_btn.setEnabled(True)

        _, stderr = self._export_proc.communicate()
        if ret == 0:
            self._set_status(f"Exported → {os.path.basename(self._export_path)}")
        else:
            msg = stderr.decode("utf-8", errors="replace").strip().removeprefix("Error: ")
            self._set_status(f"Export failed: {msg}", error=True)

    # ------------------------------------------------------------------
    # Status
    # ------------------------------------------------------------------

    def _set_status(self, msg: str, *, error: bool = False, building: bool = False):
        color = "#ff6b6b" if error else ("#888888" if building else "#4ec9b0")
        self._status_lbl.setText(msg)
        self._status_lbl.setStyleSheet(f"color: {color}; font-size: 12px;")

    # ------------------------------------------------------------------
    # Cleanup
    # ------------------------------------------------------------------

    def closeEvent(self, event):
        for proc in (self._build_proc, self._export_proc):
            if proc and proc.poll() is None:
                proc.kill()
        event.accept()


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("SDF2 Studio")
    script = sys.argv[1] if len(sys.argv) > 1 else None
    win = MainWindow(script)
    win.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
