# Device Test

An Arduino program to easily test pin over Arduino Serial.

## Usage

* Upload the code to your Arduino
* Open the serial monitor
* Send one of the following to your Arduino via Serial communication:

```arduino
// use pinMode in the specific pin
pm <pin:int> <mode:out|in>
// use digitalWrite in the pin
dw <pin:int> <val:high|low>
// use analogWrite in the pin
aw <pin:int> <val:0~255>
```

## Examples

```arduino
# Set pin 13 as output
pm 13 out
# use digitalWrite(13,HIGH)
dw 13 high
# Set pin 3 as output
pm 3 out
# use analogWrite(13,152)
aw 13 152
```
