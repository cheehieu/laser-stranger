# laser-stranger
![finished_build]()

## Introduction
For this project, I wanted to build a miniature replica of the Christmas lights wall from _Stranger Things_. If you haven't seen the first season of Stranger Things yet, add it to your watch queue ASAP! It's pretty great. 

Check out [this clip](https://youtu.be/jIQ9z2bxXyg?t=145) for a reference to the wall. Winona Ryder's character essentially uses paint and a strand of Christmas lights to create an alphabet Ouija board on her living room wall — in order to communicate with her son who is trapped in another dimension. Her son is able to spell out messages by illuminating the corresponding light bulb, letter-by-letter. 

![stranger_things_lights]()

There have been many other builds of this Stranger Things wall with some cool and creative features (e.g. [SparkFun's live Twitter wall](https://youtu.be/ZkAxEGFXsic)), but none of them really looked very accurate to the show... I wanted my replica to mimic the creepy aesthetic and be used as a "wall" art piece, so I incorporated a laser to engrave the exact paint strokes.


## Supplies
### Materials
- [C3 Christmas Lights](https://smile.amazon.com/gp/product/B07G742KHQ)
- Small Addressable RGB LEDs (I used WS2812B-2020)
- [LED Breakout Boards](https://oshpark.com/shared_projects/YB6BNPro)
- [Solid Core Wire, 22 AWG](https://smile.amazon.com/gp/product/B077HRKYB1)
- [Heat Shrink Tubing](https://smile.amazon.com/gp/product/B071H5XC7C)
- [12" x 24" Birch Plywood](https://smile.amazon.com/gp/product/B07F1WGHQW)
- [12" x 24" Frame](https://smile.amazon.com/gp/product/B003DREKD0/)
- [Sheets of 12" x 12" decorative wallpaper](https://www.romosawall.com/press-floral-wallpaper-ivory/)
- Battery-powered BLE Microcontroller Board (I used a custom-built one from an old project)
- Male and Female 0.1" Header Pins
- [Anodized Aluminum Sheet](https://smile.amazon.com/gp/product/B07CMQXWP5/)

### Tools
- Epilog Mini24 60W Laser (courtesy of Harris County Public Library)
- Soldering iron with hot air rework station
- Solder wire and solder paste
- Electric skillet
- Wire stripper/cutter
- Hot glue gun
- Glue stick
- Super glue
- Black paint and brush
- Black ink pen
- Measuring tape


## Build Process

### Laser
- create binary image of alphabet, scale image for 12"x24" (Gimp, Adobe Illustrator)
- engrave with laser
- use glue stick to glue wallpaper to plywood
- use laser to engrave alphabet onto wood (adjust settings so paper doesn't burn)
- fill in letters with black paint and small regions with ink pen
#### Step 1: 
#### Step 2: 
#### Step 3: 

### Electronics
- design breakout boards (Altium, OSH Park fabrication)
- solder LEDs onto breakout boards (stencil, electric skillet)
- measure length between letters and cut wires (VCC, GND, DATA)
- plan wire connections on the backside of frame (connectors)
- solder wires onto breakout boards
- test light strips are functional
- add heat shrink
- cut off Christmas light bulbs
- hot glue bulbs to breakout boards
#### Step 1: 
#### Step 2: 
#### Step 3: 

### Software
- mobile app user interface to press letter, type message (React Native since I use Android and recipient uses iPhone)
- BLE protocol for illuminating LEDs, ASCII alphabet
- adjustable time delays, LED brightness and color
- easter eggs
#### Step 1: 
#### Step 2: 
#### Step 3: 

### Final Assembly
- super glue LED strips to plywood, match up with letters
- insert into frame
- connect wires and male/female extension plugs
- connect to microcontroller board
- connect to mobile app
- video demo (youtube)
#### Step 1: 
#### Step 2: 
#### Step 3: 


## Gifting
- motivation as a gift for Regina
- references to Eleven (shaved head, speak slowly, communicate), using old EllieGrid electronics, a gift that only I could give
- video of unboxing
- pictures from hospital and now (2 years later)
- laser etch anodized aluminum (video)


## Resources
- https://www.instructables.com/contest/laser2021/
- Epilog laser manual settings
- 
