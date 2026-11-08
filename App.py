from flask import Flask, jsonify
from flask_cors import CORS

import serial_to_flask # Do not import latest_data directly
import logging

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR) # or logging.CRITICAL to hide almost everything

app = Flask(__name__)
CORS(app)

# Start serial reading thread
serial_to_flask.start_serial_thread()


@app.route('/api/sensor-data')
def get_sensor_data():
    return jsonify(serial_to_flask.latest_data)

if __name__ == '__main__':
    app.run(debug=True, use_reloader=False)