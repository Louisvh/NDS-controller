NDS-controller
===============
##Purpose
NDS-controller is a client app for the Nintendo DS that allows the DS to 
connect to an Android device over WiFi and to function as an input device. 
The main use case is controlling emulators: most emulated real-time games 
tends to be a pain to play on a touch screen and greatly benefit from being 
played on a gamepad. Sure, a bluetooth gamepad only costs like $50, but the 
Nintendo DS that is gathering dust in your closet is free!

## Progress
- [x] Create some sort of connection wizzard
- [x] Make it pretty
- [x] Enable connecting to a WiFi network
- [x] Create an Android keyboard that accepts network packets as input
- [ ] Fix that keyboard to not be a buggy mess and release its source code
- [ ] Translate NDS input to network packets and send them to Android device
- [ ] Add a proper options menu on both sides to configure the input

## Usage 
**NOTE:** The program doesn't actually work yet.

Build the APP into a .nds file using devkitARM. Run it using the compatible
homebrew-/flashcard of your choice. Follow the instructions on-screen to
connect it to the same WiFi network your phone is on (a tethered hotspot is 
fine too). Run the NDS-controller APP on your Android device and follow the 
instructions from there.

## LICENSE
:[MIT LICENSE](LICENSE)
