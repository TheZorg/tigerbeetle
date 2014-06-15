import datetime
import logging
from PyQt5 import Qt
from PyQt5 import QtGui
from qtibeeprogress import utils
from qtibeeprogress.qprogresswidget import QProgressWidget


logger = logging.getLogger(__name__)


class QProgressMainWindow(Qt.QMainWindow, utils.QtUiLoad):
    def __init__(self):
        super().__init__()

        self._setup_ui()

    def _add_progress_widget(self):
        layout = self._widget_central.layout()
        self._progress_widget = QProgressWidget(5, 3, 15)
        layout.insertWidget(0, self._progress_widget)

    def _setup_labels(self):
        self._labels = [
            self._lbl_begin_time,
            self._lbl_end_time,
            self._lbl_duration,
            self._lbl_cur_time,
            self._lbl_processed_events,
            self._lbl_percent,
            self._lbl_state_changes,
        ]

    def _setup_edits(self):
        self._edits = [
            self._txt_traces,
            self._txt_state_providers,
        ]

    def _setup_style(self):
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.WindowText, QtGui.QColor('#999'))

        caption_labels = [
            self._lbl_begin_time_caption,
            self._lbl_end_time_caption,
            self._lbl_duration_caption,
            self._lbl_cur_time_caption,
            self._lbl_processed_events_caption,
            self._lbl_percent_caption,
            self._lbl_traces_caption,
            self._lbl_state_changes_caption,
            self._lbl_state_providers_caption,
        ]

        for label in caption_labels:
            label.setPalette(palette)

    def _setup_ui(self):
        self._load_ui('main_wnd_progress')
        self._setup_style()
        self._setup_labels()
        self._setup_edits()
        self._add_progress_widget()
        self.adjustSize()

    def on_update_progress(self, progress):
        # ignore if invalid
        if not progress.is_valid():
            return

    def set_waiting(self):
        waiting_text = 'Waiting for data...'

        for lbl in self._labels:
            lbl.setText(waiting_text)

        for edit in self._edits:
            edit.setPlainText(waiting_text)

    @staticmethod
    def _datetime_from_ts(ts):
        # timestamps are actually in nanoseconds, so divide by 10^9
        sec_ts = ts // 1000000000

        # this will be a local datetime
        return datetime.datetime.fromtimestamp(sec_ts)

    @staticmethod
    def _ns_from_ts(ts):
        return ts - (ts // 1000000000 * 1000000000)

    @staticmethod
    def _format_time(ts):
        dt = QProgressMainWindow._datetime_from_ts(ts)
        ns = QProgressMainWindow._ns_from_ts(ts)

        dt_str = dt.strftime('%Y/%m/%d %H:%M:%S')

        text = '{} + {:09} ns'.format(dt_str, ns)

        return text

    @staticmethod
    def _format_duration(ts1, ts2):
        total_sec = (ts2 - ts1) // 1000000000
        diff_ns = (ts2 - ts1) - total_sec * 1000000000
        days = total_sec // (3600 * 24)
        total_sec -= days * (3600 * 24)
        hours = total_sec // 3600
        total_sec -= hours * 3600
        minutes = total_sec // 60
        total_sec -= minutes * 60
        seconds = total_sec

        ret = ''

        if days:
            ret += '{­} days + '.format(days)

        if hours:
            ret += '{}:{:02}'.format(hours, minutes)
        else:
            ret += '{}'.format(minutes)

        ret += ':{:02} + {} ns'.format(seconds, diff_ns)

        return ret

    def set_progress_update(self, update):
        # get begin, current and end times
        begin = update.get_begin_ts()
        begin_text = QProgressMainWindow._format_time(begin)
        end = update.get_end_ts()
        end_text = QProgressMainWindow._format_time(end)
        cur = update.get_cur_ts()
        cur_text = QProgressMainWindow._format_time(cur)

        # range (ns)
        trace_range = end - begin

        # fraction done (only considering current time)
        done = (cur - begin) / trace_range

        # processed events count
        processed_events = update.get_processed_events()

        # state changes
        state_changes = update.get_state_changes()

        # traces
        traces = update.get_traces_paths()

        # state providers
        state_providers = update.get_state_providers()

        # begin time label
        self._lbl_begin_time.setText(begin_text)

        # end time label
        self._lbl_end_time.setText(end_text)

        # duration label
        duration_text = QProgressMainWindow._format_duration(begin, end)
        self._lbl_duration.setText(duration_text)

        # current time label
        self._lbl_cur_time.setText(cur_text)

        # processed events label
        self._lbl_processed_events.setText(str(processed_events))

        # percent label
        self._lbl_percent.setText('{:.2f} %'.format(done * 100))

        # state changes label
        self._lbl_state_changes.setText(str(state_changes))

        # traces edit
        self._txt_traces.setPlainText('\n'.join(traces))

        # state providers edit
        self._txt_state_providers.setPlainText('\n'.join(state_providers))

        # progress widget value
        self._progress_widget.set_value(done)
