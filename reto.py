'''
Python Firebase Course for
Beginners
https://codeloop.org/python-firebase-course-for-beginners/
Equipo 10 - IoT
Programa Python para solucion del Reto
'''
import pyrebase

config = {
  "apiKey": "AIzaSyCK-ZdhsZuvWjMZfs8XviWai5zmkwJK2Pk",
  "authDomain": "proyecto-prueba-b3ab3.firebaseapp.com",
  "databaseURL": "https://proyecto-prueba-b3ab3-default-rtdb.firebaseio.com/",
  "projectId": "proyecto-prueba-b3ab3",
  "storageBucket": "proyecto-prueba-b3ab3.appspot.com",
  "messagingSenderId": "648022558498",
  "appId": "1:648022558498:web:3bacb2ced0b14e822dc77b",
  "measurementId": "G-5Q0VSGPBR5"
}

firebase = pyrebase.initialize_app(config)
#accesing database in firebase
db = firebase.database()
#reading some atributes of the onKey elements
all_users = db.child("app_inventor").get()

for users in all_users.each():
  print(str(users.key()), ":", str(users.val()))
  if(str(users.key()) == "digito"):
    if(int(users.val()) > 9):
      db.child("app_inventor").update({"pwm": "elemento mayor a 9"})
      print("Se modifico el valor pwm ")

