# TODO:
# - add text field with last update time (how long it took to run the callback function)
#   see https://stackoverflow.com/questions/7370801/how-to-measure-elapsed-time-in-python
# - add text field with static field of when app was started
# - add text field with last update time
# - add histogram of with all data values since start

import argparse
import sys
from queue import Queue

import pandas as pd

import rogue
from rogue.interfaces.stream import Slave

from dash.dependencies import Input, Output
import dash
import dash_core_components as dcc
import dash_html_components as html
import plotly.express as px

pd.options.mode.chained_assignment = None


class Receiver(Slave):
    """
    Simple extension of Rogue class to receive frame data
    """

    def __init__(self, buffsize=100, verbose=False):
        """
        :param maxsize: Maximum size of event buffer
        """
        Slave.__init__(self)
        self.buff = Queue(maxsize=buffsize)
        self.verbose = verbose

    def _acceptFrame(self, frame) -> None:
        """
        https://github.com/slaclab/rogue/blob/master/include/rogue/interfaces/stream/Frame.h
        :param frame:
        :return:
        """

        # This will increment byte and frame counts.
        Slave._acceptFrame(self, frame)

        data = frame.getNumpy(0, frame.getSize())

        if not self.buff.full():
            self.buff.put(data)
        else:
            # FIXME: Should the queue be emptied here?
            print("ERROR: Event buffer is full!", file=sys.stderr)

        if self.verbose:
            print("Frame: size = {}, channel = {}, firstUser = {}, lastUser = {}, flags = {}, payload = {}".format(
                frame.getSize(),
                frame.getChannel(),
                frame.getFirstUser(),
                frame.getLastUser(),
                frame.getFlags(),
                frame.getPayload()
            ))
            print("byteCount = {}, frameCount = {}".format(self.getByteCount(), self.getFrameCount()))
            print("Data: shape = {}, ndim = {}, size = {}, len = {}, min = {}, max = {}".format(
                data.shape,
                data.ndim,
                data.size,
                len(data),
                data.min(),
                data.max()
            ))
            print(str(data))

    def getEvents(self) -> list:
        evts = []
        while not self.buff.empty():
            evts.append(self.buff.get())
        return evts


parser = argparse.ArgumentParser()
parser.add_argument('-a', '--addr', help="IP address or hostname of server", default="127.0.0.1")
parser.add_argument('-p', '--port', type=int, help="Port of server", default=8000)
parser.add_argument('-n', '--npoints', type=int, help="Number of data points to display", default=25)
parser.add_argument('-i', '--interval', type=int, help="Update interval in seconds", default=1)
parser.add_argument('-d', '--debug', action='store_true',  help="Turn on debugging output", default=False)
parser.add_argument('-b', '--buffer', type=int, help="Event buffer size", default=100)
cl = parser.parse_args()

addr = cl.addr
port = cl.port
npoints = cl.npoints
update_interval = cl.interval
debug = cl.debug
buffsize = cl.buffer

df_cnt = pd.DataFrame(columns=["BytesRecd", "FramesRecd"])
client = rogue.interfaces.stream.TcpClient(addr, port)
recv = Receiver(buffsize=buffsize, verbose=cl.debug)
client._addSlave(recv)

counter_text_style = {'display': 'inline-block',
                      'font-size': 30,
                      'font-family': ['Roboto Mono', 'Arial'],
                      'border': '1px solid',
                      'vertical-align': 'top',
                      'padding': '5px',
                      'margin-top': '30px'}

app = dash.Dash()
app.layout = html.Div([
    html.H1(children="{}:{}".format(addr, port), style={'text-align': 'center'}),
    html.Div(id='bytes-counter',
             children="0",
             style=counter_text_style),
    dcc.Graph(id='bytes-graph',
              style={'display': 'inline-block'}),
    html.Div(id='frames-counter',
             children="0",
             style=counter_text_style),
    dcc.Graph(id='frames-graph',
              style={'display': 'inline-block'}),
    dcc.Graph(id='event-data-graph'),
    dcc.Graph(id='event-hist-graph'),
    dcc.Interval(id='interval-component',
                 interval=update_interval*1000,
                 n_intervals=0)
])


@app.callback(Output('bytes-counter', 'children'),
              Output('bytes-graph', 'figure'),
              Output('frames-counter', 'children'),
              Output('frames-graph', 'figure'),
              Output('event-data-graph', 'figure'),
              Output('event-hist-graph', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_graph_live(n):
    bc = recv.getByteCount()
    fc = recv.getFrameCount()
    df_cnt.loc[len(df_cnt.index)] = [bc, fc]
    df_cnt_curr = df_cnt.tail(npoints)
    fig1 = px.line(x=df_cnt_curr.index, y=df_cnt_curr['BytesRecd'].diff(), title='Bytes Received')
    fig1.update_layout(xaxis_title='Seconds', yaxis_title='Bytes', font=dict(size=18))
    fig2 = px.line(x=df_cnt_curr.index, y=df_cnt_curr['FramesRecd'].diff(), title='Frames Received')
    fig2.update_layout(xaxis_title='Seconds', yaxis_title='Frames', font=dict(size=18))

    events = recv.getEvents()

    if len(events):
        # Bar chart of values from the first event
        df = pd.DataFrame(data=events[0], columns=['value'])
        fig3 = px.bar(df, x='value', y=df.index)
        fig3.update_layout(xaxis_title='Channel', yaxis_title='Value', yaxis_range=[0, 400], xaxis_range=[0, 124],
                           font=dict(size=18), title='Single Event Data')

        # Histogram of values from all events in this update (NOT all data in session)
        data_all = pd.DataFrame(columns=['value'], dtype=int)
        for evt in events:
            data_all = data_all.append(pd.DataFrame(data=evt, columns=['value']))
        fig4 = px.histogram(data_all, x='value', nbins=50, title='All Event Data')
        fig4.update_layout(xaxis_title='Value', yaxis_title='Count', font=dict(size=18))
    else:
        raise Exception('No event data was received!')

    return bc, fig1, fc, fig2, fig3, fig4


app.run_server(debug=debug)
