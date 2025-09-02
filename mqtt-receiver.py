import paho.mqtt.client as mqtt
import json
import time
import logging

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
    main()