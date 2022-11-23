'''
Python Firebase Course for
Beginners
https://codeloop.org/python-firebase-course-for-beginners/
Equipo 10 - IoT
Programa Python para solucion del Reto
'''
import pyrebase # Importando la libreria Pyrebase

config = {  # Configurando la Firebase del Reto
  "apiKey": "AIzaSyCK-ZdhsZuvWjMZfs8XviWai5zmkwJK2Pk",
  "authDomain": "proyecto-prueba-b3ab3.firebaseapp.com",
  "databaseURL": "https://proyecto-prueba-b3ab3-default-rtdb.firebaseio.com/",
  "projectId": "proyecto-prueba-b3ab3",
  "storageBucket": "proyecto-prueba-b3ab3.appspot.com",
  "messagingSenderId": "648022558498",
  "appId": "1:648022558498:web:3bacb2ced0b14e822dc77b",
  "measurementId": "G-5Q0VSGPBR5"
}

firebase = pyrebase.initialize_app(config) # Inicializa la Configuracion de la Firebase
db = firebase.database() # Accediendo la Base de Datos de la Firebase
all_users = db.child("app_inventor").get() # Leyendo los atributos de los Elementos en sus llaves

for users in all_users.each(): # Ciclo for para recorrer todos los elementos de la base de datos con llave test
  print(str(users.key()), ":", str(users.val())) # Imprime los elementos con sus respectivas llaves
  if(str(users.key()) == "digito"): # Si la llave es el Digito
    if(int(users.val()) > 9): # Si el valor en Digito es mayor a 9
      db.child("app_inventor").update({"pwm": "elemento mayor a 9"}) # Se actualiza el elemento de usuario en la base de datos
      print("Se modifico el valor pwm ")

