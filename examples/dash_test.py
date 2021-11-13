import argparse
import rogue
from dash.dependencies import Input, Output
import dash
import dash_core_components as dcc
import dash_html_components as html
import plotly.express as px
import pandas as pd
pd.options.mode.chained_assignment = None

parser = argparse.ArgumentParser()
parser.add_argument('-a', '--addr', help="IP address or hostname of server", default="127.0.0.1")
parser.add_argument('-p', '--port', type=int, help="Port of server", default=8000)
parser.add_argument('-n', '--npoints', type=int, help="Number of data points to display", default=25)
parser.add_argument('-i', '--interval', type=int, help="Update interval in seconds", default=1)
parser.add_argument('-d', '--debug', action='store_true',  help="Turn on debugging output", default=False)
cl = parser.parse_args()

addr = cl.addr
port = cl.port
npoints = cl.npoints
update_interval = cl.interval
debug = cl.debug

df = pd.DataFrame(columns=["BytesRecd", "FramesRecd"])
client = rogue.interfaces.stream.TcpClient(addr, port)
slave = rogue.interfaces.stream.Slave()
client._addSlave(slave)

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
    dcc.Interval(id='interval-component',
                 interval=update_interval*1000,
                 n_intervals=0)
])


@app.callback(Output('bytes-counter', 'children'),
              Output('bytes-graph', 'figure'),
              Output('frames-counter', 'children'),
              Output('frames-graph', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_graph_live(n):
    bc = slave.getByteCount()
    fc = slave.getFrameCount()
    df.loc[len(df.index)] = [bc, fc]
    dftail = df.tail(npoints)
    fig1 = px.line(df, x=dftail.index, y=dftail['BytesRecd'].diff(), title='Bytes Received')
    fig1.update_layout(xaxis_title='Seconds', yaxis_title='Bytes', font=dict(size=18))
    fig2 = px.line(df, x=dftail.index, y=dftail['FramesRecd'].diff(), title='Frames Received')
    fig2.update_layout(xaxis_title='Seconds', yaxis_title='Frames', font=dict(size=18))
    return bc, fig1, fc, fig2


app.run_server(debug=debug)
