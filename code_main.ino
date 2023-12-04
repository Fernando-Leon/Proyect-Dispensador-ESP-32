

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Stepper.h>

// Stepper Motor Settings
const int stepsPerRevolution = 2048;
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// Replace with your network credentials
const char* ssid = "Leon1";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(9000);

// Search for parameters in HTTP POST request
const char* PARAM_INPUT_1 = "direction";
const char* PARAM_INPUT_2 = "portions"; // Change from "steps" to "portions"

// Variables to save values from HTML form
String direction;
String portions; // Change from "steps" to "portions"

// Variable to detect whether a new request occurred
bool newRequest = false;

// HTML to build the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Formulario de Nueva Comida</title>
    <style>

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
    
        body {
          font-family:  sans-serif;
          background-color: #f4f4f4;
          text-align: center;
          margin: 0;
          padding: 20px;
          position: relative;
        }

        .nav-bar {
            position: absolute;
            top: 0px;
            left: 0px;
            width: 100vw;
            max-height: 50px;
            overflow: hidden;
            padding: 10px;
        }

        .nav-bar ul {
            list-style: none;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100%;
            gap: 20px;
        }

        button {
            background-color: #4CAF50;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            cursor: pointer;
            font-size: 16px;
            color: #fff;
            transition: background-color 0.3s ease;
        }

        button:hover {
            background-color: #45a049;
        }
    
        main {
            height: 100vh;
            width: 100vw;
            max-height: 100vh;
            max-width: 100vw;
            display: flex;
            flex-direction: column;
            gap: 20px;
            justify-content: center;
            align-items: center;
        }
    
        h1 {
            color: #333;
            top: 0px;
            left: 0px;
            width: 100vw;
            text-align: center;
        }
    
        form, .card {
          max-width: 50vw;
          margin: 0 auto;
          background-color: #fff;
          padding: 20px;
          border-radius: 8px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
    
        input[type="radio"] {
          display: none;
        }
    
        label {
          display: inline-block;
          cursor: pointer;
          padding: 8px 16px;
          margin: 5px;
          font-size: 16px;
          color: #333;
          border-radius: 4px;
          transition: background-color 0.3s ease;
        }
    
        input[type="radio"]:checked + label {
          background-color: #4CAF50;
          color: #fff;
          border-color: #4CAF50;
        }
    
        input[type="submit"] {
          background-color: #4CAF50;
          color: #fff;
          padding: 10px 20px;
          border: none;
          border-radius: 4px;
          cursor: pointer;
          font-size: 16px;
          transition: background-color 0.3s ease;
        }
    
        input[type="submit"]:hover {
          background-color: #45a049;
        }
    
        .hide-div {
            display: none;
        }


        #form-add-new-mascota {
            display: none;
        }


        #form-historial {
            display: none;
        }


        table {
            border-collapse: collapse;
            width: 80%;
            margin: auto;
            border-radius: 10px;
            border: none;
        }

        thead {
            background-color: #4CAF50;
            color: #fff;
            border-radius: 10px;
            border: none;
        }


        th, td {
            text-align: left;
            padding: 8px;
            border-radius: 10px;
            border: none;
            padding: 5px 10px;
            margin: 5px 10px;
            gap: 10px;
        }

        tr:nth-child(even) {
            background-color: #f2f2f2;
            padding: 5px 10px;
            margin: 5px 10px;
        }

        .item-input {
            width: 100%;
            display: grid;
            grid-template-columns: 1fr 2fr;
            align-items: center;
            justify-content: space-between;
            margin: 10px 0px;
        }

        .item-input label {
            font-size: 16px;
            color: #333;
            margin: 5px 10px;
            text-align: right;
        }

        .item-input input, .item-input select {
            height: 100%;
            padding-left: 10px;
            outline: none;
            border-radius: 8px;
            border: solid 2px #4CAF50;
        }

        
    
      </style>
</head>
<body>

    <nav class="nav-bar">
        <ul>
            <li><button onclick="showInicio()">Inicio</button></li>
            <li><button onclick="showAddMascota()">Agregar Mascota</button></li>
            <li><button onclick="showHistorial()">Ver Historial</button></li>
        </ul>
    </nav>
    <main>

        <div id="form-inicio">
            <form action="/" method="POST" id="comidaForm">
                <h2>Agregar Nueva Comida a una Mascota 🐶🐈</h2>
                <div class="item-input">
                    <label for="mascotaId">Seleccionar Mascota:</label>
                    <select id="mascotaId" name="mascotaId" required>
                    </select>
                </div>
        
                <div class="hide-div">
                    <input type="radio" name="direction" id="CW" value="CW" checked>
                    <label for="CW">Clockwise</label>
                    <input type="radio" name="direction" id="CCW" value="CCW">
                    <label for="CCW">Counterclockwise</label><br><br><br>
                </div>

                <div class="item-input">
                    <label for="portions">Cantidad:</label>
                    <input type="number" id="portions" name="portions" required>
                </div>
        
    
                <input type="submit" value="Agregar Comida" id="alimentarBtn">
        
            </form>
        </div>

        <div id="form-add-new-mascota">
            
            <form id="mascotaForm">
                <h2>Ingresar Nueva Mascota ➕</h2>
                <div class="item-input">
                    <label for="nombre">Nombre:</label>
                    <input type="text" id="nombre" name="nombre" required>
                </div>
                <div class="item-input">
                    <label for="tipo">Tipo:</label>
                    <input type="text" id="tipo" name="tipo" required>
                </div>
                <div class="item-input">
                    <label for="edad">Edad:</label>
                    <input type="number" id="edad" name="edad" required>
                </div>
                <button type="button" onclick="addNewMascota()">➕ Agregar Mascota</button>
            </form>
        </div>

        <div id="form-historial">
            <div class="card">
                <h2>Historial de Comidas</h2>
                <table border="1">
                    <thead>
                        <tr>
                            <th>Nombre</th>
                            <th>Tipo</th>
                            <th>Edad (años)</th>
                            <!-- Puedes agregar más columnas según tus necesidades -->
                        </tr>
                    </thead>
                    <tbody id="mascotasTablaBody">
                        <!-- Aquí se agregarán dinámicamente las filas de la tabla -->
                    </tbody>
            </table>
            </div>
    </main>
    <script>

var mascotas = [];
fillSelect();

function getAllMascotas() {
    return fetch('http://localhost:9000/api/mascotas/')
        .then(response => response.json())
        .catch(error => console.error('Error al obtener datos:', error));
}

async function obtenerYMostrarMascotas() {
    try {
        mascotas = await getAllMascotas();
        console.log(mascotas);
    } catch (error) {
        console.error('Error al obtener y mostrar mascotas:', error);
    }
}

async function fillSelect() {
    await obtenerYMostrarMascotas(); // Espera a que obtenerYMostrarMascotas se complete
    const select = document.getElementById('mascotaId');
    let codeHtml = '';
    mascotas.forEach(mascota => {
        codeHtml += `<option value="${mascota._id}">${mascota.nombre}</option>`;
    });
    select.innerHTML = codeHtml;

    console.log("Mostrando mascotas:" + mascotas);
    console.log("Se ejecutó fillSelect");
    console.log(codeHtml);
}


function enviarFormulario() {
    const formulario = document.getElementById('comidaForm');
    const formData = new FormData(formulario);

    let data = {
        comidas: [
            {
                cantidad: formData.get('portions')
            }
        ]
    };

    data = JSON.stringify(data);

    // Obtén el ID de la mascota seleccionada desde el formulario
    const mascotaId = formData.get('mascotaId');

    fetch(`http://localhost:9000/api/mascotas/${mascotaId}/comidas`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json'
        },
        body: data
            })
            .then(response => response.json())
            .then(data => {
                console.log('Respuesta del servidor:', data);
                // Puedes agregar lógica adicional aquí, como mostrar un mensaje de éxito o redirigir a otra página.
            })
            .catch(error => console.error('Error al enviar la solicitud:', error));
        }

        document.getElementById('alimentarBtn').addEventListener('click', function (event) {
        // Evita que el formulario se envíe de inmediato
        event.preventDefault();

        // Lógica para enviar datos a la API
        enviarFormulario();

        // Deja que el formulario siga su flujo normal
        document.getElementById('comidaForm').submit();
    });


    function obtenerMascotas() {
            return fetch('http://localhost:9000/api/mascotas/')
                .then(response => response.json())
                .then(data => data)
                .catch(error => console.error('Error al obtener datos:', error));
        }

        // Esta función actualiza la tabla con las mascotas.
        function actualizarTabla() {
            const tablaBody = document.getElementById('mascotasTablaBody');

            // Limpia el contenido actual de la tabla
            tablaBody.innerHTML = '';

            // Obtén las mascotas desde el servidor
            obtenerMascotas().then(mascotas => {
                // Itera sobre las mascotas y agrega filas a la tabla
                mascotas.forEach(mascota => {
                    const fila = document.createElement('tr');

                    const nombreCelda = document.createElement('td');
                    nombreCelda.textContent = mascota.nombre;
                    fila.appendChild(nombreCelda);

                    const tipoCelda = document.createElement('td');
                    tipoCelda.textContent = mascota.tipo;
                    fila.appendChild(tipoCelda);

                    const edadCelda = document.createElement('td');
                    edadCelda.textContent = mascota.edad;
                    fila.appendChild(edadCelda);

                    // Agrega la fila a la tabla
                    tablaBody.appendChild(fila);
                });
            });
        }

        function addNewMascota() {
            const formulario = document.getElementById('mascotaForm');
            const formData = new FormData(formulario);

            let data = {
                nombre: formData.get('nombre'),
                tipo: formData.get('tipo'),
                edad: formData.get('edad'),
                comidas: [] 
            };
            data = JSON.stringify(data);

            fetch('http://localhost:9000/api/mascotas/', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: data
            })
            .then(response => response.json())
            .then(data => {
                console.log('Respuesta del servidor:', data);
                alert("Mascota agregada con éxito");
            })
            .catch(error => console.error('Error al enviar la solicitud:', error));
        }


        function showInicio() {
            document.getElementById("form-inicio").style.display = "block";
            document.getElementById("form-add-new-mascota").style.display = "none";
            document.getElementById("form-historial").style.display = "none";
        }

        function showAddMascota() {
            document.getElementById("form-inicio").style.display = "none";
            document.getElementById("form-add-new-mascota").style.display = "block";
            document.getElementById("form-historial").style.display = "none";
            actualizarTabla();
        }

        function showHistorial() {
            document.getElementById("form-inicio").style.display = "none";
            document.getElementById("form-add-new-mascota").style.display = "none";
            document.getElementById("form-historial").style.display = "block";
            actualizarTabla();  
        }



    </script>

</body>
</html>

)rawliteral";

// Initialize WiFi
void initWiFi() {
  // Implement WiFi initialization if needed
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  myStepper.setSpeed(5);

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // Handle request (form)
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if (p->name() == PARAM_INPUT_1) {
          direction = p->value().c_str();
          Serial.print("Direction set to: ");
          Serial.println(direction);
        }
        // Change from "steps" to "portions"
        if (p->name() == PARAM_INPUT_2) {
          portions = p->value().c_str();
          Serial.print("Number of portions set to: ");
          Serial.println(portions);
        }
      }
    }
    request->send(200, "text/html", index_html);
    newRequest = true;
  });

  server.begin();
}

void loop() {
  // Check if there was a new request and move the stepper accordingly
  if (newRequest){
    // Change from "steps" to "portions"
    int stepsToMove = portions.toInt() * stepsPerRevolution;
    if (direction == "CW"){
      myStepper.step(stepsToMove);
    }
    else {
      myStepper.step(-stepsToMove);
    }
    newRequest = false;
  }
}

