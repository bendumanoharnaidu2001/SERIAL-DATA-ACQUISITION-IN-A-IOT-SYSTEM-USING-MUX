from django.db import models

# Create your models here.
class devices(models.Model):
   device_id =  models.PositiveIntegerField(unique=True)
   p0_status = models.CharField(max_length=3, default = 'OFF')
   relay_status = models.CharField(max_length=3, default = 'OFF')
   led_status = models.CharField(max_length=6, default = 'OFF')


class users(models.Model):
   email = models.EmailField(max_length=256, unique=True)
   password = models.CharField(max_length=64)
   google_enabled =  models.CharField(max_length=1, default='N')
   google_refresh_token = models.CharField(max_length=256, blank=True, null=True, default=None)
   google_access_token = models.CharField(max_length=256, blank=True, null=True, default=None)
