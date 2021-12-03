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

![laser](https://user-images.githubusercontent.com/1174029/144571166-d778d2d4-8883-4915-b52a-b783ca9c6e78.jpg)
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

![wire_measure](https://user-images.githubusercontent.com/1174029/144547162-ae63bbd9-e4f6-49f3-b6bf-f464ae1a3ccb.JPG)

Careful planning must be done to measure the wires between each LED. You must account for the space between the letters, but also leave room for wire stripping and soldering. It was helpful to use a measuring tape and a wire stripper/cutter for this task. To make it easier to mount the Chrismas lights to the plywood, I decided to split the lights into three separate strands, one for each row of letters. This makes it look cleaner from the front, while the daisy-chain connection is managed in the back behind the frame. For this, I used 0.1" male and female header pins to effectively make extension cables.

![wire_led_top_](https://user-images.githubusercontent.com/1174029/144566073-920d9c65-9f9f-4f60-8878-3ee49b681949.jpg)
![wire_led_bottom_](https://user-images.githubusercontent.com/1174029/144566070-b8e0c170-fb4d-4248-894c-240b3f88cb32.jpg)
![wire_header](https://user-images.githubusercontent.com/1174029/144566305-a49f3b41-20ea-4314-8d09-04b4248ed329.jpg)

I also added heat shrink tubing between each LED to make it look like a single black cable, matching how Christmas lights normally look. After soldering the wires to the breakout boards, use hot air to activate the heat shrink tubing.

![wire_heat_shrink](https://user-images.githubusercontent.com/1174029/144547192-6e8ee3f6-a3e0-4eb3-a80e-f9c7d2a58366.JPG)


### Step 4: Mount Christmas Light Bulbs
As you are adding new LEDs to the daisy-chain, it is good practice to test that the LEDs are functional and in the correct position. An easy way to check this is by connecting the strand to an Arduino and using Adafruit's NeoPixel library strandtest example to illuminate the LEDs. Once all of the LEDs have been verified to work, you are ready to mount the Christmas light bulbs to the breakout boards.

![wire_measure_lights](https://user-images.githubusercontent.com/1174029/144547441-e8a8ee6f-bc4c-4261-acbf-736b13f2a920.JPG)
![wire_test_lights](https://user-images.githubusercontent.com/1174029/144547445-4c6afa35-6ec0-4671-a944-afa52aea63f4.JPG)

The C3 Christmas lights that I bought didn't have easily removable plastic bulbs. I needed to pry them off and use a cutter to carefully expose a large enough hole at the bulb base for the LED to shine through. Without cutting the plastic base, the LED would not properly diffuse light inside the bulb; it needed to be flush to maximize light throw. Once 26 bulbs were removed, I arranged them in a color order to avoid any same-color neighbors. Using a hot glue gun, add a dab of hot glue on top of the LED and quickly apply the plastic bulb to the breakout board. Hold together for a few seconds for the surfaces to bond. The hot glue acts as an adhesive, but also works to diffuse light inside the bulb. Repeat this glueing process for all remaining LEDs, also using super glue if a stronger bond is needed. Pay attention to the orientation of each bulb, as some need to point at different angles.

![bulb_remove](https://user-images.githubusercontent.com/1174029/144566530-f1fa36bd-8374-4ae7-8e15-33a8ebb8dba1.jpg)
![bulb_extract](https://user-images.githubusercontent.com/1174029/144566529-936ec566-9f09-4bc6-acf0-78c88d298035.jpg)
![wire_glue_bulbs](https://user-images.githubusercontent.com/1174029/144547438-5718d957-8050-4190-9397-c5f66aeda50e.JPG)
![wire_measure_bulbs](https://user-images.githubusercontent.com/1174029/144547439-8bd377bc-e796-4e76-8418-f0fc825b8845.JPG)

After mounting the bulbs, test the light strand again to see how the bulbs look when illuminated.

![wire_test_bulbs](https://user-images.githubusercontent.com/1174029/144547443-7a2f0b39-fe1b-4538-9838-91a9135eb5aa.JPG)


## Software
The control software consists of microcontroller firmware to drive the LEDs and respond to Bluetooth commands from a mobile application. I used custom-built electronics from a previous project because it already contained the necessary components to power several LEDs from a Lithium-polymer battery. This board used a Nordic nRF52832 BLE System-on-Chip, which could easily connect to any Android or Apple smartphone device with Bluetooth capabilities. 

### Step 1: Alphabet-LED Protocol Firmware

#### LEDs
|  # | Character | ASCII Dec | ASCII Hex | ASCII Dec | ASCII Hex |
|:--:|:---------:|:---------:|:---------:|:---------:|:---------:|
|  - |   Space   |     32    |    0x20   |     --    |     --    |
|  0 |    A, a   |     65    |    0x41   |     97    |    0x61   |
|  1 |    B, b   |     66    |    0x42   |     98    |    0x62   |
|  2 |    C, c   |     67    |    0x43   |     99    |    0x63   |
|  3 |    D, d   |     68    |    0x44   |    100    |    0x64   |
|  4 |    E, e   |     69    |    0x45   |    101    |    0x65   |
|  5 |    F, f   |     70    |    0x46   |    102    |    0x66   |
|  6 |    G, g   |     71    |    0x47   |    103    |    0x67   |
|  7 |    H, h   |     72    |    0x48   |    104    |    0x68   |
|  8 |    I, i   |     73    |    0x49   |    105    |    0x69   |
|  9 |    J, j   |     74    |    0x4A   |    106    |    0x6A   |
| 10 |    K, k   |     75    |    0x4B   |    107    |    0x6B   |
| 11 |    L, l   |     76    |    0x4C   |    108    |    0x6C   |
| 12 |    M, m   |     77    |    0x4D   |    109    |    0x6D   |
| 13 |    N, n   |     78    |    0x4E   |    110    |    0x6E   |
| 14 |    O, o   |     79    |    0x4F   |    111    |    0x6F   |
| 15 |    P, p   |     80    |    0x50   |    112    |    0x70   |
| 16 |    Q, q   |     81    |    0x51   |    113    |    0x71   |
| 17 |    R, r   |     82    |    0x52   |    114    |    0x72   |
| 18 |    S, s   |     83    |    0x53   |    115    |    0x73   |
| 19 |    T, t   |     84    |    0x54   |    116    |    0x74   |
| 20 |    U, u   |     85    |    0x55   |    117    |    0x75   |
| 21 |    V, v   |     86    |    0x56   |    118    |    0x76   |
| 22 |    W, w   |     87    |    0x57   |    119    |    0x77   |
| 23 |    X, x   |     88    |    0x58   |    120    |    0x78   |
| 24 |    Y, y   |     89    |    0x59   |    121    |    0x79   |
| 25 |    Z, z   |     90    |    0x5A   |    122    |    0x80   |

#### Parameters
- LED number (0-25, uint8_t)
- LED color (RGB, uint8_t x 3) [led_r, led_g, led_b]
- Sequence LED on duration [char_duration] (ms, uint16_t)
- Sequence LED off duration [inter_char_duration] (ms, uint16_t)
- 'Space' character duration [space_duration] (ms, uint16_t)

### Step 2: User Interface
I apologize for the user interface design... I didn't spend much time on it so it is purely functional (not pretty). The app has four main sections that are accessible via the bottom tabs. _Wall_ lets you interact with the alphabet lights in real-time; pressing a letter in the app will illuminate the corresponding letter in the frame. _Message_ lets you input a text string to display letter-by-letter like in the show. The function iterates over the character array and activates the corresponding LED in sequence. By default, the light-on character duration is 1000ms, the light-off time between characters is 250ms, and the light-off time between words is 1000ms. _Light Patterns_ contains some easter eggs to recreate the messages from the show ("right here", "run") and some other fun things like a Magic 8 Ball. _Settings_ gives you full control to monitor the battery level and adjust the time and LED color/brightness settings. Using max brightness white is easiest because the light bulbs are already colored.
- [App Demo with Test Lights](https://youtu.be/60Hty-S8RYM)

### Step 3: Easter Eggs
- mobile app user interface to press letter, type message (React Native since I use Android and recipient uses iPhone)
- BLE protocol for illuminating LEDs, ASCII alphabet
- adjustable time delays, LED brightness and color
- easter eggs



## Final Assembly
### Step 1: Glue Christmas Lights
Position the Christmas lights strands against the wallpaper plywood to align with the letters and match the configuration of the real Stranger Things wall. Use super glue for a more secure and permanent bond.

![assembly_glue](https://user-images.githubusercontent.com/1174029/144567320-39fe976a-a276-41ac-8043-37e3532b1237.JPG)

### Step 2: Insert Into Frame
Fold the wires around to the backside of the plywood and insert the piece into the frame. Secure with the frame tabs and clear room for any wall-mounting hardware.

![assembled_lights_off](https://user-images.githubusercontent.com/1174029/144567691-84be48a6-fddf-45f1-9fe3-4731ded8443b.JPG)

### Step 3: Connect Electronics
Connect the light strands together with the extension cables and plug it into the electronics board. Connect the battery or power the board via USB cable.

![assembled_back](https://user-images.githubusercontent.com/1174029/144567552-d4403e16-796e-43f7-94dd-b6bf2150a153.JPG)

### Step 4: Send a Message
Launch the mobile app and establish a Bluetooth connection. 
- ["hello world"](https://youtu.be/1JN2lHtpktk)
- ["RUN"](https://youtu.be/DlZCyXovSxo)
- ["David"](https://youtu.be/lXHU-I2KuCw)


# Gifting
The primary motivation for this project was to be a gift for my friend and coworker, Regina. A few months prior, she had a hemorrhagic stroke that required multiple brain surgeries. She experienced paralysis on the right side of her body and had some difficulty speaking. We were both fans of the Stranger Things show and agreed that she resembled one of the characters with her shaved head and slow speech (Eleven).

- using old EllieGrid electronics, a gift that only I could give
- video of unboxing (https://youtu.be/BQxKGPLLM7Q)
- pictures from hospital and now (2 years later)
- laser etch anodized aluminum (https://youtu.be/YgBGavpdD-0)


# Resources
- https://blog.hackster.io/get-ready-for-season-2-of-stranger-things-with-your-own-alphabet-wall-242b6fe39044
- https://github.com/sparkfun/Stranger_Things_Wall
- https://www.instructables.com/id/Stranger-Things-Interactive-Wall-Art/
- https://www.instructables.com/contest/laser2021/
- 
