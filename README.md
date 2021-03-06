# Pico SoundTracker/ProTracker modplayer

6 March 2021

This is a super-simple Sunday (OK, Saturday)-afternoon hack plugging my [Little Mod Player](https://github.com/evansm7/lmp) into Mike Field's I2S/DMA code from his [PiPico Drummer](https://github.com/hamsternz/PiPico_Drummer.git) project.

When combined with a [Pimoroni Pico Audio Pack](https://shop.pimoroni.com/products/pico-audio-pack), this allows you to play a [SoundTracker](https://en.wikipedia.org/wiki/Ultimate_Soundtracker) or [ProTracker](https://en.wikipedia.org/wiki/ProTracker) module (song) from your Pico!

This is useful for:

 * Re-living the early 1990s or, if you weren't there, empathising with those who were and the crude technology we enjoyed
 * Pretending you have an Amiga in your pocket (not recommended)
 * Getting to know demoscene music a bit better
 * Enjoying the aliasing of 8-bit samples
 * Making a terrible terrible portable music player, so hip it hurts

And:

 * Easily add a soundtrack to your Pico demos and games.
 * Play music with low CPU overhead, and with low storage requirements:  the player is about 2.5KB, and you'll find fun MODs in the 100-200KB range.


## Getting started/building

Set up your Pick SDK environment (not doc'd here), and point to it, e.g.:

~~~
export PICO_SDK_PATH=/home/l33t/src/pico-sdk
export PATH=$PATH:/usr/local/gcc-arm-embedded/
~~~

Grab a cool SoundTracker (or ProTracker) modfile (MOD not included), and turn it into a header file:

~~~
# For example!
curl https://api.modarchive.org/downloads.php?moduleid=33453#amiga.mod > modfile.mod

xxd -i < modfile.mod > modfile.h
~~~

Set up cmake, and build the project:

~~~
cmake CMakeLists.txt
make
~~~

Copy the resulting `pmp.uf2` to your Pico, and enjoy the sound.  Though, that depends on what MOD file you choose.  Maybe it'll be awful.  There's no accounting for taste.

Note: If you want to change the song after building for the first time, just replace `modfile.h` and then do a `make`.


## Operation

After setup, LMP creates a new buffer of stereo signed 16-bit PCM samples from the song when a buffer fill is required.  This is not currently interrupt-driven for this Saturday-afternoon hack, but do try that -- a "background" interrupt-driven player is possible.

The songs themselves have 4 channels and play 8-bit samples; they are linearly interpolated by LMP to upsample to the I2S 44.1KHz/16-bit output rate.  The songs are played in stereo with a slightly softer mix than the Amiga hard left-right selection for each channel.


## Notes

This is .... pretty rough and ready.  No features or quality, please move along.

Also, I don't want to sound like your mum but there is no volume control and the output to _my_ headphones at least is _super-loud_.  Don't screw up your hearing, please.  When you play any new MOD file, take your headphones out _before_ it starts!

If (anyone uses this and) you ask sweetly I'll consider adding a simple volume control to LMP.  :)

Also, if you use anyone's tracker module in your productions, be cool and **credit its author**!


## Project ideas

Integrate into a game, or demo.

Switch between different songs.

Load (small-medium) songs from an SD card.

Output sound wave visualiser on your fancy OLED screen.

Volume control, fast forward/rewind.

Tweet me if you make something!


## Getting more music to play

Check out (and support?) the [Mod Archive](https://modarchive.org/index.php).  Other tracker mod archives are available.  The internet and BBSes are full of 'em.

Look for "15-instrument" or "31-instrument" SoundTracker or ProTracker files.  They'll usually have a `.MOD` suffix.  XM, S3M, IT etc. are a different format and not compatible with LMP.

Note the LMP player is really noddy and won't correctly play super-complex MODs.

Even better, write your own!  There are various ProTracker-compatible programs available for all OSes.

I was recently impressed by the web-based [BassoonTracker](https://www.stef.be/bassoontracker/), very cool.

* * *

(c) 2021 Matt Evans
