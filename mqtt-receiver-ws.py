# import paho.mqtt.client as mqtt
# import json
# import time
# import logging
# from flask import Flask
# import socketio
# import threading

# # === Logger setup ===
# logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
# logger = logging.getLogger(__name__)

# # === MQTT Configuration ===
# MQTT_BROKER = 'broker.hivemq.com'
# MQTT_PORT = 1883
# MQTT_TOPIC = 'rru_2026/iot/sensors/data'
# MQTT_CLIENT_ID = "python-mqtt-reader"

# # === Socket.IO server setup ===
# sio = socketio.Server(cors_allowed_origins='*')  # Allow all origins
# app = Flask(__name__)
# app.wsgi_app = socketio.WSGIApp(sio, app.wsgi_app)

# # === MQTT Callbacks ===
# def on_connect(client, userdata, flags, rc):
#     responses = {
#         0: "Connected successfully",
#         1: "Incorrect protocol version",
#         2: "Invalid client ID",
#         3: "Server unavailable",
#         4: "Bad username or password",
#         5: "Not authorized"
#     }
#     logger.info(f"MQTT: {responses.get(rc, 'Unknown error')}")
#     if rc == 0:
#         client.subscribe(MQTT_TOPIC)
#         logger.info(f"Subscribed to {MQTT_TOPIC}")

# def on_disconnect(client, userdata, rc):
#     if rc != 0:
#         logger.warning("MQTT: Unexpected disconnection. Will auto-reconnect.")
#     else:
#         logger.info("MQTT: Disconnected successfully.")

# def on_message(client, userdata, msg):
#     try:
#         payload = msg.payload.decode("utf-8")
#         logger.info(f"MQTT: Message on {msg.topic}: {payload}")
        
#         try:
#             data = json.loads(payload)
#             logger.info(f"Parsed JSON: {data}")
            
#             # Emit data to all connected WebSocket clients
#             sio.emit('sensor_data', data)
#             logger.info("Data emitted to Socket.IO clients.")
#         except json.JSONDecodeError:
#             logger.warning("Received non-JSON payload")

#     except Exception as e:
#         logger.error(f"Error handling MQTT message: {e}")

# # === MQTT Client Thread Function ===
# def start_mqtt():
#     client = mqtt.Client(client_id=MQTT_CLIENT_ID)
#     client.on_connect = on_connect
#     client.on_message = on_message
#     client.on_disconnect = on_disconnect

#     logger.info(f"Connecting to MQTT broker {MQTT_BROKER}:{MQTT_PORT}")
#     client.connect(MQTT_BROKER, MQTT_PORT, 60)
#     client.loop_forever()

# # === Run everything ===
# if __name__ == "__main__":
#     # Start MQTT in a separate thread
#     mqtt_thread = threading.Thread(target=start_mqtt)
#     mqtt_thread.daemon = True
#     mqtt_thread.start()

#     logger.info("Starting Socket.IO server on http://localhost:5000")
#     app.run(host='0.0.0.0', port=5000)

import paho.mqtt.client as mqtt
import json
import time
import logging
from flask import Flask
import socketio
import threading

# Set up logging configuration
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

MQTT_BROKER = 'broker.hivemq.com'
MQTT_PORT = 1883
MQTT_TOPIC = 'rru_2026/iot/sensors/data'
MQTT_CLIENT_ID = "python-mqtt-reader"
MQTT_USERNAME = None  # Set if your broker requires authentication
MQTT_PASSWORD = None  # Set if your broker requires authentication

sio = socketio.Server(cors_allowed_origins='*')  # Allow all origins
app = Flask(__name__)
app.wsgi_app = socketio.WSGIApp(sio, app.wsgi_app)

# Define MQTT callbacks
def on_connect(client, userdata, flags, rc):
    connection_responses = {
        0: "Connected successfully",
        1: "Incorrect protocol version",
        2: "Invalid client ID",
        3: "Server unavailable",
        4: "Bad username or password",
        5: "Not authorized"
    }
    message = connection_responses.get(rc, f"Unknown error code: {rc}")
    logger.info(f"MQTT: {message}")
    
    if rc == 0:
        # Subscribe to topic upon successful connection
        client.subscribe(MQTT_TOPIC)
        logger.info(f"Subscribed to {MQTT_TOPIC}")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        logger.warning("MQTT: Unexpected disconnection. Will auto-reconnect")
    else:
        logger.info("MQTT: Disconnected successfully")

def on_message(client, userdata, msg):
    try:
        topic = msg.topic
        payload = msg.payload.decode("utf-8")
        logger.info(f"Received message on topic {topic}: {payload}")
        
        # Try to parse as JSON if it's in that format
        try:
            data = json.loads(payload)
            logger.info(f"Parsed JSON: {data}")
            
            # Here you can process the data as needed
            # For example, if data contains sensor readings:
            if isinstance(data, dict) and "value" in data:
                sio.emit('sensor_data', data["value"])
                logger.info(f"Sensor value: {data['value']}")
        except json.JSONDecodeError:
            # Not JSON format, handle as plain text
            logger.info(f"Raw message (not JSON): {payload}")
    
    except Exception as e:
        logger.error(f"Error processing message: {e}")

def main():
    # Create MQTT client instance
    client = mqtt.Client(client_id=MQTT_CLIENT_ID)
    
    # Set callback functions
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    
    # Set up authentication if provided
    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    
    try:
        # Connect to HiveMQ broker
        logger.info(f"Connecting to MQTT broker {MQTT_BROKER} on port {MQTT_PORT}")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        
        # Start network loop
        client.loop_start()
        
        # Keep the script running
        logger.info("MQTT reader is now running. Press Ctrl+C to exit.")
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        logger.info("Process interrupted by user")
    except Exception as e:
        logger.error(f"Error in main loop: {e}")
    finally:
        # Clean up on exit
        logger.info("Cleaning up connections...")
        client.loop_stop()
        if client.is_connected():
            client.disconnect()
        logger.info("MQTT client disconnected")

if __name__ == "__main__":
    
    # Start MQTT in a separate thread
    mqtt_thread = threading.Thread(target=main)
    mqtt_thread.daemon = True
    mqtt_thread.start()

    logger.info("Starting Socket.IO server on http://localhost:5000")
    app.run(host='0.0.0.0', port=5000)