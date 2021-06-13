# Commands
        
## Menus

|Command | Action               |
|--------|----------------------|
| *0#    | Sleep mode |
| *100#  | Main menu |
| *101#  | Show splash screen |
| *102#  | Account balance menu |
| *103#  | Read card menu |
| *104#  | Pay menu |
        
## Testing

|Command | Action               |
|--------|----------------------|
| *7000# | Dump firmware info |
| *7001# | Server status check |
| *7002# | Check connection and authentication |
| *7003# | Pay 0 JMF: Payment test |
| *7004# | LED color test |
| *7005# | PWM LED spectrum fade test |
| *7006# | Buzzer self test |
| *7007# | RFID self test |

## Resets

|Command | Action               |
|--------|----------------------|
| *8001# | Reset RC522 |
| *8002# | Re-init RC522 (hard reset) |

## Advanced Settings

> DO NOT USE THEM UNLESS YOU KNOW WHAT YOU'RE DOING

|Command | Action                          |
|--------|---------------------------------|
| *9101# | Set RC522 antenna gain to 18 dB |
| *9102# | Set RC522 antenna gain to 33 dB |
| *9103# | Set RC522 antenna gain to 48 dB |

## Setup mode and factory reset

|Command   | Action                          |
|----------|---------------------------------|
| Hold * on boot | Enters setup mode |
| *707172# | Forces setup mode |
| *909192# | Resets EEPROM and sets setup mode flag |
