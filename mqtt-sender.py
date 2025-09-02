import serial
import time
import json
import paho.mqtt.client as mqtt
import logging
from datetime import datetime
from functools import reduce

# Set up logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# MQTT callbacks
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

def on_disconnect(client, userdata, rc):
    if rc != 0:
        logger.warning("MQTT: Unexpected disconnection. Trying to reconnect...")
    else:
        logger.info("MQTT: Disconnected successfully")

def on_publish(client, userdata, mid):
    logger.debug(f"MQTT: Message {mid} published")

def main():
    # Hard-coded configuration
    SERIAL_PORT = 'COM15'
    BAUD_RATE = 9600
    MQTT_HOST = 'broker.hivemq.com'
    MQTT_PORT = 1883
    MQTT_TOPIC = 'rru_2026/iot/sensors/data'
    MQTT_USERNAME = None  # Set to None if not using authentication
    MQTT_PASSWORD = None  # Set to None if not using authentication
    READ_INTERVAL = 1.0   # Seconds between readings
    
    # Set up MQTT client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_publish = on_publish
    
    # Set up authentication if provided
    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    
    try:
        logger.info(f"Connecting to MQTT broker at {MQTT_HOST}:{MQTT_PORT}")
        client.connect(MQTT_HOST, MQTT_PORT, 60)
        client.loop_start()
        
        # Set up serial connection
        logger.info(f"Opening serial port {SERIAL_PORT} at {BAUD_RATE} baud")
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        
        # Give the serial connection time to establish
        time.sleep(2)
        
        # Clear any initial data
        ser.flushInput()
        
        logger.info("Starting sensor data collection...")
        while True:
            try:
                # Read a line from serial port
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').strip()
                    logger.debug(f"Received: {line}")
                    
                    try:
                        # Try to parse as JSON if it's in that format
                        data = json.loads(line)
                    except json.JSONDecodeError:
                        # Otherwise use the raw data as a string
                        data = {"value": line, "timestamp": datetime.now().isoformat()}

                    # Convert data to JSON string and publish
                    def red(acc, el):
                        k, v = el.split(":")
                        acc[k] = v
                        return acc
                    
                    data["value"] = reduce(red, data["value"].split(","), {})
                    payload = json.dumps(data)
                    logger.info(f"Publishing: {payload}")
                    client.publish(MQTT_TOPIC, payload)
                
                time.sleep(READ_INTERVAL)
                
            except KeyboardInterrupt:
                logger.info("Interrupted by user")
                break
            except Exception as e:
                logger.error(f"Error reading/publishing data: {e}")
                time.sleep(1)  # Wait before retry
    
    except Exception as e:
        logger.error(f"Error in main loop: {e}")
    
    finally:
        logger.info("Cleaning up...")
        if 'ser' in locals() and ser.is_open:
            ser.close()
            logger.info("Serial port closed")
        
        if client.is_connected():
            client.loop_stop()
            client.disconnect()
            logger.info("MQTT client disconnected")

if __name__ == "__main__":
    main()