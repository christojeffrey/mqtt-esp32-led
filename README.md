# what is this

basic MQTT and ESP32 implementation
main objective: on trigger will update LED blinking frequency, and update that frequency value to MQTT broker.

# how to run

1. run MQTT broker (on container using mosquitto) using `podman-compose up` or `docker-compose up`. I tested this using podman.
2. setup your ESP32 like [this](https://esp32io.com/tutorials/esp32-ultrasonic-sensor)
3. run `main/main.ino` on your ESP32 using Arduino IDE (don't forget to setup pubsubclient library and esp32 board on your Arduino IDE)

# doc

there are demo videos inside `doc` folder. 