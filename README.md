# datalogger-nodemcu-firebase

- Exemplo de comunicação entre Módulo Wifi ESP8266 NodeMCU ESP-12E e a plataforma Google Firebase  usando Arduino IDE. 

- Este projeto faz uso de Firebase Authentication e cria um sistema de data logging no Firebase Real Time Database. Os dados são gerados aleatoriamente para simulação de 2 sensores e faz uso da biblioteca NTPClient para conectar-se a um servidor NTPC e obter uma marca temporal. 

Recebimento de dados no Firebase Real Time Database ~> https://firebase.google.com/:

<div align="center">
    <img src=./images/firebase.png width="100%">
</div>

- Os dados são agrupados no nó readings de acordo com o user UID autenticado e o timestamp obtido a partir da conexão NTPC no momento do envio.
