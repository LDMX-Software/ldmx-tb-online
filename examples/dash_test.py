import plotly.express as px
import dash
import dash_core_components as dcc
import dash_html_components as html
import pandas as pd
from dash.dependencies import Input, Output

import rogue

df = pd.DataFrame(columns=["BytesRecd", "FramesRecd"])

client = rogue.interfaces.stream.TcpClient("127.0.0.1", 8000)
slave = rogue.interfaces.stream.Slave()
client._addSlave(slave)

app = dash.Dash()
app.layout = html.Div([
    html.Div(id='total-bytes'),
    dcc.Graph(id='bytes-graph'),
    html.Div(id='total-frames'),
    dcc.Graph(id='frames-graph'),
    dcc.Interval(id='interval-component',
        interval=1*1000,
        n_intervals=0)
])

@app.callback(Output('bytes-graph', 'figure'),
              Output('frames-graph', 'figure'),
              Output('total-bytes', 'children'),
              Output('total-frames', 'children'),
              Input('interval-component', 'n_intervals'))
def update_graph_live(n):
    global df
    bc = slave.getByteCount()
    fc = slave.getFrameCount()
    df.loc[len(df.index)] = [bc, fc]
    df = df.tail(50)
    fig1 = px.line(df, x=df.index, y=df['BytesRecd'].diff(), title='Bytes Received')
    fig2 = px.line(df, x=df.index, y=df['FramesRecd'].diff(), title='Frames Received')
    return fig1, fig2, bc, fc

app.run_server(debug=True) 
