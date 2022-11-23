'''
Python Firebase Course for
Beginners
https://codeloop.org/python-firebase-course-for-beginners/
Equipo 10 - IoT
Este programa se utiliza para dar solucion a una de las secciones del Reto
Consiste en leer la base de datos, mostrar los elementos, y modificarla segun
Una decision tomada en base al valor del Digito
'''
import pyrebase # Importando la libreria Pyrebase

config = {  # Configurando la Firebase del Reto
  "apiKey": "AIzaSyAzXLpt54jZP6Q3yUQoEYq2A-v_0vDQix8",
  "authDomain": "si-quieres-te-la-saco.firebaseapp.com",
  "databaseURL": "https://si-quieres-te-la-saco-default-rtdb.firebaseio.com",
  "projectId": "si-quieres-te-la-saco",
  "storageBucket": "si-quieres-te-la-saco.appspot.com",
  "messagingSenderId": "1095716845856",
  "appId": "1:1095716845856:web:8cb684d054ee5c2ddd3f45",
  "measurementId": "G-MSZ11G7GG9"
}

firebase = pyrebase.initialize_app(config) # Inicializa la Configuracion de la Firebase
db = firebase.database() # Accediendo la Base de Datos de la Firebase
all_users = db.child("test").get() # Leyendo los atributos de los Elementos en sus llaves

for users in all_users.each(): # Ciclo for para recorrer todos los elementos de la base de datos con llave test
  print(str(users.key()), ":", str(users.val())) # Imprime los elementos con sus respectivas llaves
  if(str(users.key()) == "digito"): # Si la llave es el Digito
    if(int(users.val()) > 9): # Si el valor en Digito es mayor a 9
      db.child("test").update({"usuario": "elemento mayor a 9"}) # Se actualiza el elemento de usuario en la base de datos
      print("Se modifico el valor de usuario ")

