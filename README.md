NDS-controller
===============

3DS .cia link, scan in FBI to install:

![v1 2 0qr](https://user-images.githubusercontent.com/6605273/31918707-655dbfc8-b85e-11e7-9d66-0a5043f072d7.png)

[Link to Android host App](https://play.google.com/store/apps/details?id=com.ldvhrtn.ndscontroller)

## Purpose
NDS-controller is a client app for the Nintendo DS that allows the DS to 
connect to an Android device over WiFi and to function as an input device. 
The main use case is controlling emulators: most emulated real-time games 
tends to be a pain to play on a touch screen and greatly benefit from being 
played on a gamepad. Sure, a Bluetooth gamepad only costs like $50, but the 
Nintendo DS that is gathering dust in your closet is free!

I started this project long ago, shelved it after losing interest 
and un-shelved it after buying a 3DS. The NDS version started as an exercise 
in making something pretty and functional, but I lost interest before it became 
functional. I decided to skip the "pretty" step in the 3DS version and got it
working first. The "pretty" may come at some later time.


## Progress
- [x] Create some sort of connection wizard
- [x] Make it pretty
- [x] Enable connecting to a WiFi network
- [x] Create an Android keyboard that accepts network packets as input
- [x] Translate NDS input to network packets and send them to Android device
- [x] Write a 3DS version
- [ ] Make that 3DS version pretty
- [ ] Add analog support for the 3DS o-pad
- [ ] Add a proper options menu to configure the input
- [x] Open-source it all

**NOTE:** A flashcart or CFW is required on your (3)DS!

## Usage 

*NDS:*
Build the client into a .nds file using devkitARM or download it __[here](https://github.com/Louisvh/NDS-controller/releases/download/v1.2.0/NDS-controller.nds)__.
Run it using the compatible homebrew-/flashcard of your choice. Follow the 
instructions on-screen to connect it to the same WiFi network your phone is 
on (a tethered hotspot is fine too). Run the NDS-controller app on your 
Android device and follow the instructions from there.

*3DS:*
Build the client using devkitARM or download it from __[here](https://github.com/Louisvh/NDS-controller/releases/download/v1.2.0/NDS-controller.cia)__. Install it 
using your preferred method. Run the app in Android, match your 3DS client 
to the IP address displayed on the screen and follow the instructions on the 
Android app.


## LICENSE
:[MIT LICENSE](LICENSE)
