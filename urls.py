from django.urls import path
from . import views

app_name = 'myapp'

urlpatterns = [
    path('', views.index, name='index'),
    path('index/', views.index, name='index'),
    path('login/', views.login, name='login'),
    path('googleAuth/', views.googleAuth, name='googleAuth'),
    path('googleToken/', views.googleToken, name='googleToken'),
    path('googleHome/', views.googleHome, name='googleHome'),
]
