# laser-stranger
![framed_one_light_on](https://user-images.githubusercontent.com/1174029/144354003-ca20dbc3-cf82-4640-ae4c-c142b6783144.JPG)

# Introduction
For this project, I wanted to build a miniature replica of the Christmas lights wall from _Stranger Things_. If you haven't seen the first season of Stranger Things yet, do yourself a favor and add it to your watch queue ASAP! It's pretty great. 

Check out [this video clip](https://youtu.be/jIQ9z2bxXyg?t=145) for a reference to the wall. Winona Ryder's character essentially uses black paint and a strand of Christmas lights to create an alphabet Ouija board on her living room wall—in order to communicate with her son who is trapped in another dimension. Her son is able to spell out messages by illuminating the corresponding light bulbs, letter-by-letter.

![wall_from_show_poster](https://user-images.githubusercontent.com/1174029/144354030-8ee90029-8864-4493-968d-61834825d295.jpg)

Many other makers have already built versions of this Stranger Things wall with some neat and creative features (e.g. [SparkFun's live Twitter wall](https://youtu.be/ZkAxEGFXsic)), but none of them really looked very accurate to the show... I wanted my replica to mimic the creepy aesthetic and be used as a "wall" art piece, so I incorporated a laser to engrave the exact paint strokes. 

The finished product was given as a gift to my friend, Regina. It featured addressable LEDs, a battery-powered Bluetooth microcontroller, and could be controlled by a companion mobile application to display messages. 


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
- Tweezers
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
### Step 1: Select LEDs and Design Breakout Boards
In order to illuminate one light bulb at a time, the light strand needs to use addressable LEDs. I found small C3-sized Christmas lights on Amazon, but they were not addressable... so I decided to retrofit the bulbs with small NeoPixel RGB LEDs. These LEDs are addressable and very popular among electronics hobbyists. Each LED would need to be daisy-chained with 3 wires to provide 3V, ground, and a data signal. Knowing this, I designed small breakout printed circuit boards that could support the C3 bulb and connect each LED to the daisy-chain with wires. The breakout board PCB was designed in Altium and fabricated by OSH Park.

![oshpark_top](https://user-images.githubusercontent.com/1174029/144363480-a4315248-3df3-442f-9236-60ed5d836052.png) ![oshpark_bottom](https://user-images.githubusercontent.com/1174029/144363478-235aaebc-cd5a-4edf-88a9-6e7d3e483fe0.png)
![breakout_boards](https://user-images.githubusercontent.com/1174029/144363537-aa680d75-93f9-496d-a9a8-fccf468c5e57.JPG)

### Step 2: Assemble Breakout Boards
These LEDs and breakout boards are quite small. Instead of soldering the heat-sensitive parts by hand, I decided to use solder paste and an electric skillet (the poor man's reflow oven). I found it helpful to use a microscope and stencil to apply the paste to the pads, then use tweezers to carefully place the LED. I didn't bother with soldering the decoupling capacitor to allow more space for mounting the light bulb.

![breakout_board](https://user-images.githubusercontent.com/1174029/144365111-db85b88e-0379-465f-bb17-ed6cef285b37.JPG) ![breakout_stencil](https://user-images.githubusercontent.com/1174029/144365177-1bcdf8dd-55a5-400d-88b7-8fbf2f947bca.JPG)
![breakout_solder_paste](https://user-images.githubusercontent.com/1174029/144365174-aa02af1c-7593-46cb-ad16-deb7dc90debe.JPG) ![breakout_led_placement](https://user-images.githubusercontent.com/1174029/144365162-a9ee8354-3b9b-4c4a-ba5a-a6c97576a11b.JPG)

Repeate this process at least 26 times...

![breakout_solder_paste_repeat](https://user-images.githubusercontent.com/1174029/144365170-f315eb2d-36ce-40e5-8585-8d903ceb7f1e.JPG)

After all the boards are populated, place them on the electric skillet. Cover and turn up the heat. Try to follow the reflow profile for the LEDs, but ultimately keep an eye on the solder paste and watch when it gets hot enough to make a solid joint. I like using low-temp solder paste for this.

![breakout_skillet](https://user-images.githubusercontent.com/1174029/144365165-8e818b87-efb7-4901-8de0-61ff2de42c04.JPG)
![breakout_assembled](https://user-images.githubusercontent.com/1174029/144365157-9f174783-e5ce-4cfd-8a9e-fcc327bdf2c4.JPG)


### Step 3: Wire Up
The next step is to add wires to connect each LED breakout board together. These addressable LEDs work in a daisy-chain configuration with 3 input signals (VDD, GND, DIN) and 3 output signals (VDD, GND, DOUT). To keep track of the signals, I color-coded the wires using the standard convention of VDD=red, GND=black, DATA=white. I selected solid core 22 AWG gauge wire because it is easy to solder to through-hole pads and the wire itself holds its shape when bent.



Careful planning must be done to measure the wires between each LED. You must account for the space between the letters, but also leave room for wire stripping and soldering. It was helpful to use a measuring tape and a wire stripper/cutter for this task. To make it easier to mount the Chrismas lights to the plywood, I decided to split the lights into three separate strands, one for each row of letters. This makes it look cleaner from the front, while the daisy-chain connection is managed in the back behind the frame. For this, I used 0.1" male and female header pins to make effective extension cables with removable connections.



I also added heat shrink tubing between each LED to make it look like a single black cable, matching how Christmas lights normally look. After soldering the wires to the breakout boards, use hot air to activate the heat shrink tubing.



### Step 4: Mount Christmas Light Bulbs
As you are adding new LEDs to the daisy-chain, it is good practice to test that the LEDs are functional and in the correct position. An easy way to check this is by connecting the strand to an Arduino and using Adafruit's NeoPixel library strandtest example to illuminate the LEDs. Once all of the LEDs have been verified to work, you are ready to mount the Christmas light bulbs to the breakout boards.

The C3 Christmas lights that I bought didn't have easily removable plastic bulbs. I couldn't simply unscrew or pry them off. I had to use a cutter to carefully cut them off, exposing a large enough hole at the bulb base for the LED to shine through. Once 26 bulbs were removed, I arranged them in a color order to avoid any same-color neighbors. Using a hot glue gun, add a dab of hot glue on top of the LED and quickly apply the plastic bulb to the breakout board. Hold together for a few seconds for the surfaces to bond. The hot glue acts as an adhesive, but also works to diffuse light inside the bulb. Repeat this glueing process for all remaining LEDs, also using super glue if a stronger bond is needed. Pay attention to the orientation of each bulb, as some need to point at different angles.



After mounting the bulbs, test the light strand again to see how the bulbs become illuminated.



## Software
The control software consists of microcontroller firmware to drive the LEDs and respond to Bluetooth commands from a mobile application. I used custom-built electronics from a previous project because it already contained the necessary components to power several LEDs from a Lithium-polymer battery. This board used a Nordic nRF52832 BLE System-on-Chip, which could easily connect to any Android or Apple smartphone device with Bluetooth capabilities. 

### Step 1: Alphabet-LED Protocol Firmware


### Step 2: User Interface


### Step 3: Easter Eggs
- mobile app user interface to press letter, type message (React Native since I use Android and recipient uses iPhone)
- BLE protocol for illuminating LEDs, ASCII alphabet
- adjustable time delays, LED brightness and color
- easter eggs


## Final Assembly
### Step 1: Glue Christmas Lights
Position the Christmas lights strands against the wallpaper plywood to align with the letters and match the configuration of the real Stranger Things wall. Use super glue for a more secure and permanent bond.

### Step 2: Insert Into Frame
Fold the wires around to the backside of the plywood and insert the piece into the frame. Secure with the frame tabs and clear room for any wall-mounting hardware.

### Step 3: Connect Electronics
Connect the light strands together with the extension cables and plug it into the electronics board. Connect the battery or power the board via USB cable.

### Step 4: Send a Message
Connect to the mobile app and start sending messages.
- video demo (https://youtu.be/60Hty-S8RYM)
- hello world (https://youtu.be/1JN2lHtpktk)
- run (https://youtu.be/DlZCyXovSxo)
- david sux ()


# Gifting
The primary motivation for this project was to be a gift for my friend and coworker, Regina. A few months prior, she had a hemorrhagic stroke that required multiple brain surgeries. She experienced paralysis on the right side of her body and had some difficulty speaking. We were both fans of the Stranger Things show and agreed that she resembled one of the characters with her shaved head and slow speech (Eleven).

- using old EllieGrid electronics, a gift that only I could give
- video of unboxing (https://youtu.be/BQxKGPLLM7Q)
- pictures from hospital and now (2 years later)
- laser etch anodized aluminum (video)


# Resources
- https://www.instructables.com/contest/laser2021/
- 
