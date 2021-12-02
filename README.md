# laser-stranger
![framed_one_light_on](https://user-images.githubusercontent.com/1174029/144354003-ca20dbc3-cf82-4640-ae4c-c142b6783144.JPG)

# Introduction
For this project, I wanted to build a miniature replica of the Christmas lights wall from _Stranger Things_. If you haven't seen the first season of Stranger Things yet, do yourself a favor and add it to your watch queue ASAP! It's pretty great. 

Check out [this video clip](https://youtu.be/jIQ9z2bxXyg?t=145) for a reference to the wall. Winona Ryder's character essentially uses black paint and a strand of Christmas lights to create an alphabet Ouija board on her living room wall—in order to communicate with her son who is trapped in another dimension. Her son is able to spell out messages by illuminating the corresponding light bulbs, letter-by-letter.

![wall_from_show_poster](https://user-images.githubusercontent.com/1174029/144354030-8ee90029-8864-4493-968d-61834825d295.jpg)

Many other makers have already built versions of this Stranger Things wall with some neat and creative features (e.g. [SparkFun's live Twitter wall](https://youtu.be/ZkAxEGFXsic)), but none of them really looked very accurate to the show... I wanted my replica to mimic the creepy aesthetic and be used as a "wall" art piece, so I incorporated a laser to engrave the exact paint strokes. 

The finished product was given as a "Get well soon!" gift to my friend, Regina. It featured addressable LEDs, a battery-powered Bluetooth microcontroller, and could be controlled by a companion mobile application to display messages. 


# Supplies
![supplies](https://user-images.githubusercontent.com/1174029/144354061-fc948fe1-e66d-489a-b540-ea46305cba26.JPG)

## Materials
- [C3 Christmas Lights](https://smile.amazon.com/gp/product/B07G742KHQ)
- Small Addressable RGB LEDs (I used WS2812B-2020)
- [LED Breakout Boards](https://oshpark.com/shared_projects/YB6BNPro)
- [Solid Core Wire, 22 AWG](https://smile.amazon.com/gp/product/B077HRKYB1)
- [Heat Shrink Tubing](https://smile.amazon.com/gp/product/B071H5XC7C)
- [12" x 24" Birch Plywood](https://smile.amazon.com/gp/product/B07F1WGHQW)
- [12" x 24" Frame](https://smile.amazon.com/gp/product/B003DREKD0/)
- [Sheets of 12" x 12" Decorative Paper](https://www.romosawall.com/press-floral-wallpaper-ivory/) (I couldn't find the exact wallpaper design)
- Battery-powered BLE Microcontroller Board (I used a custom-built one from an old project)
- Male and Female 0.1" Header Pins
- [Anodized Aluminum Sheet](https://smile.amazon.com/gp/product/B07CMQXWP5/)

## Tools
- Epilog Mini 24 60W Laser (courtesy of Harris County Public Library)
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


# Build Process
This project mainly consists of three separate parts: laser engraving the alphabet, constructing a custom Christmas lights strand, and writing the control software. The most tedious part was definitely the lights strand because it was all done manually by hand... and there are 26 letters. But I'm pretty proud with how it all turned out.

## Laser Alphabet
### Step 1: Create Binary Image
In order to do the laser engraving, we need to provide a black/white image to the Epilog laser software. Since I only want to engrave the alphabet paint strokes, they must be segmented from the rest of the wall (Christmas lights, wallpaper, etc.) in the source image. I used Gimp and Adobe Illustrator image editing tools to adjust the threshold levels and erase any unwanted artifacts. Then I resized the binary image to fit my 12" x 24" canvas.

![wall_from_show](https://user-images.githubusercontent.com/1174029/144355141-9549f3a1-f3ae-4618-997d-a9257df9c37a.png)
![stranger_things_wall_12x24](https://user-images.githubusercontent.com/1174029/144355174-6eaa01c5-e9fe-4e6d-91ba-6f44241f0b2d.png)
_Note: the 'Z' character was originally omitted in my source image, which is why it doesn't quite match up..._

### Step 2: Prepare Canvas
Next, prepare the canvas by glueing wallpaper to the plywood. I used a glue stick and wallpaper samples that had a similar floral design as the one from the show.

![wallpaper_plywood](https://user-images.githubusercontent.com/1174029/144361759-4bdf11ba-9a60-4ef7-a2f2-298d816305fd.JPG)
![wallpaper_plywood_glued](https://user-images.githubusercontent.com/1174029/144359100-5434d9df-c20c-4e49-bd60-042c01dd4292.JPG)

### Step 3: Laser Engraving
Make sure to test your laser engraving settings on a piece of scrap plywood first! You'll want to dial in the speed/power settings to etch through the paper and a layer of plywood, but not burn it! I did not do this and got burn marks on my expensive wallpaper... so I had to start over using 2 sheets of 12"x12" decorative paper. Also, make sure the canvas is properly aligned on the bed. Then, just let the laser do the work!

Unfortunately, I did not have the foresight to record a video of the laser engraving.

![wallpaper_engrave](https://user-images.githubusercontent.com/1174029/144360150-4fc4c89c-ab6e-4679-9abf-62990722078a.JPG)

### Step 4: Paint the Letters
Fill in the engraved letters with black paint. Be careful not to drip any extra paint marks. I also found it helpful to use a black ink pen to touch up the narrow regions in the wood. 

![wallpaper_paint](https://user-images.githubusercontent.com/1174029/144360478-1b1536e9-b6ec-4667-86e8-f1909f7566e5.JPG)
![wallpaper_paint_fill](https://user-images.githubusercontent.com/1174029/144360471-b4842591-c69e-4a59-a4fa-5a8687cabcc8.JPG)


## Christmas Lights
### Step 1: 
### Step 2: 
### Step 3: 
- design breakout boards (Altium, OSH Park fabrication)
- solder LEDs onto breakout boards (stencil, electric skillet)
- measure length between letters and cut wires (VCC, GND, DATA)
- plan wire connections on the backside of frame (connectors)
- solder wires onto breakout boards
- test light strips are functional
- add heat shrink
- cut off Christmas light bulbs
- hot glue bulbs to breakout boards

## Software
### Step 1: 
### Step 2: 
### Step 3: 
- mobile app user interface to press letter, type message (React Native since I use Android and recipient uses iPhone)
- BLE protocol for illuminating LEDs, ASCII alphabet
- adjustable time delays, LED brightness and color
- easter eggs

## Final Assembly
- super glue LED strips to plywood, match up with letters
- insert into frame
- connect wires and male/female extension plugs
- connect to microcontroller board
- connect to mobile app
- video demo (youtube)
### Step 1: 
### Step 2: 
### Step 3: 


# Gifting
- motivation as a gift for Regina
- references to Eleven (shaved head, speak slowly, communicate), using old EllieGrid electronics, a gift that only I could give
- video of unboxing
- pictures from hospital and now (2 years later)
- laser etch anodized aluminum (video)


# Resources
- https://www.instructables.com/contest/laser2021/
- Epilog laser manual settings
- 
