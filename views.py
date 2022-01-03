# IOT course
#
from django.shortcuts import render, Http404
from django.http import HttpResponseRedirect, JsonResponse
from django.urls import reverse
from django.views.decorators.csrf import csrf_exempt

import random
import string
import json

import paho.mqtt.client as mqtt

from .models import devices
from .models import users
from .forms import loginForm

# Create your views here.
def index(request):
      #If no record exists in data base, create a record.  This happens only the first time
      try:
         users_obj = users.objects.get(email='BENDUMANOHARNAIDU@GMAIL.COM')
      except:
         users_obj = users()
         users_obj.email = 'BENDUMANOHARNAIDU@GMAIL.COM'
         users_obj.password = "Test@1234"
         users_obj.save()

      user_email = None
      if 'user_email' in request.session:
         user_email = request.session['user_email']
         if user_email != 'BENDUMANOHARNAIDU@GMAIL.COM':
            user_email = None
      if not user_email:
         return HttpResponseRedirect(reverse('myapp:login'))

      #If no record exists in data base, create a record.  This happens only the first time
@
                                                                                                   
 # Note:  It is assumed that you have only one device.  If you have more than one device, this code has to be modified.  You will have to do that yourself after completing this course.

      try:
         devices_obj = devices.objects.get(device_id=1)
      except:
         devices_obj = devices()
         devices_obj.device_id = 1
         devices_obj.p0_status = "OFF"
         devices_obj.relay_status = "OFF"
         devices_obj.led_status = "OFF"
         devices_obj.save()

      p0_status = devices_obj.p0_status
      relay_status = devices_obj.relay_status
      led_status = devices_obj.led_status

      p0_status_new = request.GET.get("p0_status")
      relay_status_new = request.GET.get("relay_status")
      led_status_new = request.GET.get("led_status")
      buzzer = request.GET.get("buzzer")

      message = ''
      msg = ""
      if p0_status_new == "ON":
         msg = "P0:ON"
      elif p0_status_new == "OFF":
         msg = "P0:OFF"
      if relay_status_new == "ON":
         msg = "RELAY:ON"
      elif relay_status_new == "OFF":
         msg = "RELAY:OFF"
      if led_status_new == "ON":
         msg = "LED:ON"
      elif led_status_new == "OFF":
         msg = "LED:OFF"
      if buzzer == "BUZZER":
         msg = "BUZZER:ON"

      sensors = device_send_rec_msg(msg)
      if sensors:
         message = 'Success'
         if p0_status_new:
            p0_status = p0_status_new
            devices_obj.p0_status = p0_status
         if relay_status_new:
            relay_status = relay_status_new
            devices_obj.relay_status = relay_status
         if led_status_new:
            led_status = led_status_new
            devices_obj.led_status = led_status
         devices_obj.save()

      else:
         message = 'Fail'

      context = {
         'p0_status': p0_status,
         'relay_status': relay_status,
         'led_status': led_status,
         'sensors': sensors,
         'message': message,
      }
      return render(request, 'myapp/ports_status.html', context)


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

def on_publish(client, userdata, message):
   global published
   published = True



def login(request):
   if request.method == 'POST':
      form = loginForm(request.POST)
      email = None
      if form.is_valid():
         email = form.cleaned_data['email']
        if email:
            email = email.upper()
            password = form.cleaned_data['password']
            try:
               users_obj = users.objects.get(email=email)
               user_password = users_obj.password
               if password != user_password:
                  users_obj = None
            except:
               users_obj = None
            if users_obj:
               request.session['user_email'] = email
               my_session = None
               if 'my_session' in request.session:
                  my_session = request.session['my_session']
               if my_session == 'google':
                  redirect_uri = request.GET.get('redirect_uri')
                  state = request.GET.get('state')
                  authorization_code = email.encode().hex()
                  redirect_uri = redirect_uri+'?code='+authorization_code+'&state='+state
                  return HttpResponseRedirect(redirect_uri)
               return HttpResponseRedirect(reverse('myapp:index'))
            else:
               Error_msg = "No such user"
         else:
            Error_msg = "No such user"
      else:
         Error_msg = "Invalid inputs"
      context = {
         'Error_msg': Error_msg,
         'form': form,
         'email': email,
      }
      return render(request, 'myapp/login_form.html', context)
   else:
      Error_msg = request.GET.get('msg')
      form = loginForm()
      email = None
      context = {
         'Error_msg': Error_msg,
         'form': form,
         'email': email,
      }
      return render(request, 'myapp/login_form.html', context)



def googleAuth(request):
   print("AUTH Call")
   redirect_uri = request.GET.get('redirect_uri')
   google_client_id = request.GET.get('client_id')
   state = request.GET.get('state')
   scope = request.GET.get('scope')
   response_type = request.GET.get('response_type')
   if not state:
      state = ''

   GOOGLEHOME_CLIENT_ID = 'MyGoogleHomeClientId'
   if google_client_id == GOOGLEHOME_CLIENT_ID and response_type == 'code':
      request.session['my_session'] = 'google'
      msg = "Login to WiFiSecureAccess. By Signing in, you are authorizing Google to access your devices."
      args = '%s' + '?msg=%s&action=googlehome&redirect_uri=%s&state=%s' % (msg, redirect_uri, state)
      return HttpResponseRedirect(args  % reverse('myapp:login'))

   else:
      raise Http404



@csrf_exempt
def googleToken(request):
   print("TOKEN Call")
   if request.method == 'POST':
      client_id = request.POST.get('client_id')
      client_secret = request.POST.get('client_secret')
                                                                                                                                                                   225,1         41%                                                                                                                                                                    150,1         32% 
     grant_type = request.POST.get('grant_type')
      code = request.POST.get('code')
      refresh_token = request.POST.get('refresh_token')

      GOOGLEHOME_CLIENT_ID = 'MyGoogleHomeClientId'
      GOOGLE_CLIENT_SECRET = 'Test@1234'
      if client_id == GOOGLEHOME_CLIENT_ID and client_secret == GOOGLE_CLIENT_SECRET:
         if grant_type == 'authorization_code':
            user_email = None
            if code:
               user_email = bytes.fromhex(code).decode()
            if user_email and user_email == "BENDUMANOHARNAIDU@GMAIL.COM":
               try:
                  users_obj = users.objects.get(email=user_email)
               except:
                  users_obj = None
               if users_obj:
                  users_obj.google_enabled = 'Y'
                  refresh_token = code + ':' + ''.join(random.sample((string.ascii_uppercase + string.ascii_lowercase + string.digits + "#$%&*?"), 16))
                  access_token = code + ':' + ''.join(random.sample((string.ascii_uppercase + string.ascii_lowercase + string.digits + "#$%&*?"), 16))
                  users_obj.google_refresh_token = refresh_token
                  users_obj.google_access_token = access_token
                  users_obj.save()
                  data = {
                     'token_type': 'Bearer',
                     'access_token': access_token,
                     'refresh_token': refresh_token,
                     'expires_in': 3600,
                  }
                  return JsonResponse(data)
               else:
                  return JsonResponse(status=400, data={"error": "invalid_authorization_code"})
            else:
               return JsonResponse(status=400, data={"error": "invalid_authorization_code"})

         elif grant_type == 'refresh_token':
            users_obj = None
            try:
            params = refresh_token.split(':')
               email_code = params[0]
               user_email = bytes.fromhex(email_code).decode()
               users_obj = users.objects.get(email=user_email)
            except:
               users_obj = None
            if users_obj and users_obj.google_enabled == 'Y':
               if refresh_token == users_obj.google_refresh_token:
                  access_token = str(email_code) + ':' + ''.join(random.sample((string.ascii_uppercase + string.ascii_lowercase + string.digits + "#$%&*?"), 16))
                  users_obj.google_access_token = access_token
                  users_obj.save()
                  data = {
                     'token_type': 'Bearer',
                     'access_token': access_token,
                     'expires_in': 3600,
                  }
                  return JsonResponse(data)
               else:
                  return JsonResponse(status=401, data={"error": "invalid_refresh_token"})
            else:
               return JsonResponse(status=401, data={"error": "invalid_refresh_token"})
         else:
            return JsonResponse(status=401, data={"error": "invalid_grant"})
      else:
            return JsonResponse(status=401, data={"error": "invalid_request"})
   else:
      return JsonResponse(status=401, data={"error": "invalid_call"})



#For allowing authorization header, Add the following line at the end of /etc/apache2/apache2.conf
# Add this line just befor the line AccessFileName .htaccess
#Then restart apache2
#WSGIPassAuthorization On
@csrf_exempt
def googleHome(request):
   print("GoogleHome ")
   if request.method == 'POST':
      data = json.loads(request.body.decode("utf-8"))
      access_token_str = request.headers.get('Authorization')
      access_token = access_token_str.replace('Bearer', '')
      access_token = access_token.replace(' ', '')

      users_obj = None
      try:
         params = access_token.split(':')
         email_code = params[0]
         user_email = bytes.fromhex(email_code).decode()
         users_obj = users.objects.get(email=user_email)
      except:
         users_obj = None
      if users_obj and users_obj.google_enabled == 'Y':
         if access_token == users_obj.google_access_token:
            requestId = data['requestId']
            intent_str = data['inputs'][0]['intent']
            intent = intent_str.split('.')

            if intent[0] == 'action' and intent[1] == 'devices' and intent[2] == 'SYNC':
               print("Google SYNC")

               user_devices = list()
               payload = {
                  "agentUserId": email_code,
                  "devices": user_devices,
               }

               response = {
                  "requestId": requestId,
                  "payload": payload,
               }

               device1 = {
                  "id": "P0",
                  "type": "action.devices.types.SWITCH",
                  "traits": ["action.devices.traits.OnOff",],
                  "name": {
                   "name": "P0",
                  },
                  "willReportState": False,
                  "roomHint": "My Course",
                  "attributes": {},
                  "deviceInfo": {
                     "manufacturer": "Myself",
                     "model": "1",
                     "hwVersion": "1",
                     "swVersion": "1"
                  }
               }
               user_devices.append(device1)

               device2 = {
                  "id": "RELAY",
                  "type": "action.devices.types.OUTLET",
                  "traits": ["action.devices.traits.OnOff",],
                  "name": {
                      "name": "RELAY",
                  },
                  "willReportState": True,
                  "roomHint": "My Course",
                  "attributes": {},
                  "deviceInfo": {
                     "manufacturer": "Myself",
                     "model": "1",
                     "hwVersion": "1",
                     "swVersion": "1"
                  }
               }
               user_devices.append(device2)

               #print(response)
               return JsonResponse(response)

            elif intent[0] == 'action' and intent[1] == 'devices' and intent[2] == 'QUERY':
               print("QUERY")
               #print(data)
               payload = data["inputs"][0]["payload"]
               request_devices = payload["devices"]

               response_devices = {}
               devices_obj = devices.objects.get(device_id=1)
               for request_device in request_devices:
                  out_port = request_device["id"]  # Note: in our case, id is the name of the output port
                  dev_state = {}
                  if out_port == "P0":
                     dev_state["online"] = True
                     if devices_obj.p0_status == 'ON':
                        dev_state["on"] = True
                     else:
                        dev_state["on"] = False
                     dev_state["status"] = 'SUCCESS'
                     response_devices['P0'] = dev_state
                  elif out_port == "RELAY":
                     dev_state["online"] = True
                     if devices_obj.relay_status == 'ON':
                        dev_state["on"] = True
                     else:
                        dev_state["on"] = False
                     dev_state["status"] = 'SUCCESS'
                     response_devices['RELAY'] = dev_state

               response_payload = {
                    "devices": response_devices,
               }
               response = {
                  "requestId": requestId,
                  "payload": response_payload,
               }
               #print(response)
               return JsonResponse(response)

            elif intent[0] == 'action' and intent[1] == 'devices' and intent[2] == 'EXECUTE':
               print("EXECUTE")
               payload = data["inputs"][0]["payload"]
               commands = payload["commands"]
               out_commands = list()
               for command in commands:
                  cust_devices = command["devices"]
                  execution = command["execution"][0]
                  execution_command = execution["command"]
                  #Note:  In our case, the ports are OnOff type and so, execution command will ne OnOff
                  # If you have ports with other types and traits, you have to check here
                  if "OnOff" in execution_command:
                     exe_params = execution["params"]
                     exe_action = None
                     if "on" in exe_params:
                        if exe_params["on"]:
                           exe_action = 'ON'
                        else:
                           exe_action = 'OFF'
                     if exe_action:
                        devices_obj = devices.objects.get(device_id=1) # From data base
                        for cust_device in cust_devices:
                           Error_code = 0
                           out_port = cust_device["id"]

                           out_device = {}
                           out_device["ids"] = [out_port]
                           out_dev_state = {}

                           msg = out_port + ":" + exe_action
                           sensors = device_send_rec_msg(msg) # execute the request
                           if sensors:
                              if out_port == "P0":
                                 devices_obj.p0_status = exe_action
                                 devices_obj.save()
                              elif out_port == "RELAY":
                                 devices_obj.relay_status = exe_action
                                 devices_obj.save()
                              if exe_action == 'ON':
                                 out_dev_state["on"] = True
                         else:
                                 out_dev_state["on"] = False
                              out_dev_state["online"] = True
                              out_device["status"] = "SUCCESS"
                           else:
                              out_dev_state["on"] = False
                              out_dev_state["online"] = False
                              out_device["errorCode"] = "deviceOffline"
                              out_device["status"] = "ERROR"

                           out_device["states"] = out_dev_state
                  out_commands.append(out_device)

               out_payload = {
                  "commands": out_commands,
               }
               response = {
                  "requestId": requestId,
                  "payload": out_payload,
               }
               #print(response)
               return JsonResponse(response)

            elif intent[0] == 'action' and intent[1] == 'devices' and intent[2] == 'DISCONNECT':
               #print("DISCONNECT")
               users_obj.google_enabled = 'N'
               users_obj.save()
               return JsonResponse({})

            else:
               return JsonResponse(status=401, data={"error": "invalid_intent"})
         else:
            return JsonResponse(status=401, data={"error": "invalid_intent"})
      else:
         return JsonResponse(status=401, data={"error": "invalid_intent"})
   else:
      #print("GET Method")
      return JsonResponse(status=401, data={"error": "invalid_grant"})
                                                                                                                                                                   491,1         99% 
                                                                                                                                                                   453,1         91% 
                                                                                                                                                                   415,1         82% 
                                                                                                                                                                   377,1         74% 
                                                                                                                                                                   339,1         66%                                                                                                                                                                    301,1         57%                                                                                                                                                                    263,1         49% 
                                                                                                                                                                   150,1         24% 
