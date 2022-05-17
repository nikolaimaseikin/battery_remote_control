import sqlite3
from flask import Flask, render_template, request, url_for, flash, redirect, make_response
from data import db_session
from data.datas import Datas
#import matplotlib.pyplot as plt
import datetime
import numpy as np
from flask_socketio import SocketIO, emit

RESISTANCE = 1000

app = Flask(__name__)
app.config['SECRET_KEY'] = '8lLgGgnWBXFR'

@app.route('/')
def index():
    db_sess = db_session.create_session()
    sp = []
    for i in db_sess.query(Datas).all():
        sp.append(str(i).split())
    return render_template('index.html', uploads=sp)

@app.route('/graph')
def graph():
    db_sess = db_session.create_session()
    sp = []
    for i in db_sess.query(Datas).all():
        sp.append(str(i).split())
    events = []
    readings = []
    for i in sp:
        readings.append(float(i[1]))
        stt = i[2] + ' ' + i[3][:8]
        events.append(datetime.datetime.strptime(stt, '%Y-%m-%d %H:%M:%S'))
    phi = np.linspace(0, 2.*np.pi, 100)
    plt.plot(phi, np.sin(phi))
    #name = 'foo.jpg'
    #plt.savefig(f'static/img/{name}')
    #plt.show()
    return render_template('graph.html')

    
@app.route('/<int:id>')
def uniq_upl(id):
    db_sess = db_session.create_session()
    sp = []
    for i in db_sess.query(Datas).filter(Datas.id == id):
        sp.append(str(i).split())
    if sp == []:
        abort(404)
    return render_template('uniq.html', upl=sp)


@app.route('/insert', methods=['POST', 'GET'])
def create():
    content = request.get_json()
    if content:
        if content["mac"] == "24:a1:60:30:4c:71":
            voltage = content["voltage"]
            amperage = (voltage / RESISTANCE) * 1000
            db_sess = db_session.create_session()
            data = Datas()
            data.voltage = voltage
            data.amperage = amperage
            db_sess.add(data)
            db_sess.commit()
            db_sess.close()
            return make_response(f"voltage is {voltage}; amperage is {amperage}", 200)

if __name__ == "__main__":
    db_session.global_init("db/datas.db")
    db_sess = db_session.create_session()

    app.run(host='0.0.0.0')