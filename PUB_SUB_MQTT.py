#!/usr/bin/env python3
# This to be used lesson_10
#

import paho.mqtt.client as mqtt
import time

def main_fn():
   p0_status = 'OFF'
   relay_status = 'OFF'
   led_status = 'OFF'

   while True:
      input_str = None
      while not input_str:
         input_str = input("Enter command: ")

      p0_status_new = None
      relay_status_new = None
      led_status_new = None

      input_str = input_str.upper()
      msg = ""
      if 'RELAY' in input_str:
         if 'ON' in input_str:
            relay_status_new = "ON"
            msg = 'RELAY:ON'
         elif 'OFF' in input_str:
            relay_status_new = "OFF"
            msg = 'RELAY:OFF'

      elif 'LED' in input_str:
         if 'OFF' in input_str:
            led_status_new = "OFF"
            msg = 'LED:OFF'
         elif 'ON' in input_str:
            led_status_new = "ON"
            msg = 'LED:ON'

      elif 'BUZZER' in input_str:
         msg = 'BUZZER:ON'
      sensors = device_send_rec_msg(msg)
      if sensors:
         message = 'Success'
         if p0_status_new:
            p0_status = p0_status_new
         if relay_status_new:
            relay_status = relay_status_new
         if led_status_new:
            led_status = led_status_new

      else:
         message = 'Fail'

      print(message)
      print("P0: ", p0_status)
      print("RELAY: ", relay_status)
      print("LED: ", led_status)
      print(sensors)


def device_send_rec_msg(msg):
      broker_address = "127.0.0.1"
      mqtt_username = "manu"
      mqtt_password = "1234"
      client = mqtt.Client("SERVER")
      client.username_pw_set(mqtt_username, mqtt_password) # Sets the username and password  
      client.on_message=on_message #attach function to callback
      client.on_publish = on_publish
      client.connect(broker_address) #connect to broker
      client.subscribe('SENSORS')  # All sensor value
      global published
      published = None
      global rec_msg
      rec_msg = None
      client.publish("DEVICE", msg)
      count = 50
      while count > 0:
         client.loop(0.1) # This wait for 0.1 seconds
         count -= 1
         if published:
            break
      if published:
         count = 50 # Wait a maximum of 5 seconds.  1 count is 0.1 seconds
         while count > 0:
            client.loop(0.1) # Waits within the loop for 0.1 seconds
            count -= 1
            if rec_msg:
               break
      client.loop_stop()
      client.disconnect()
      return rec_msg

def on_message(client, userdata, message):
    global rec_msg
    rec_msg = message.payload.decode("utf-8")
    print("Receive_message",rec_msg)
def on_publish(client, userdata, message):
   global published
   published = True


if __name__ == '__main__':
  main_fn()
