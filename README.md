# GameBoy Emulator
![Super Mario Land](https://i.imgur.com/XoIT385.gif)
GameBoy emulator written in C++ using SFML graphics library.
Capable of running Tetris and Super Mario Land currently.

## Getting Started

Check instructions below for setting up the project on your local machine

### Prerequisites

To get started clone the project from this repository and install
development environment of your choise.

```
git clone https://github.com/JoniSuominen/GBEmu.git
```

### Installing

Clone the repository into your computer if you already haven't: 

```
git clone https://github.com/JoniSuominen/GBEmu.git
```

Download SFML for your development environment of choise: 
```
https://www.sfml-dev.org/download/sfml/2.5.1/
```

Download SFML for your development environment of choise: 
```
https://www.sfml-dev.org/download/sfml/2.5.1/
```

Setup SFML with development environment of choise: 
```
https://www.sfml-dev.org/tutorials/2.5/
```

## Blargg's tests

Blargg's tests check how well the emulator matches
with the original GameBoy by checking return values of
cpu instructions and timings of operations.
The tests can be found on internet but I will not provide
links for them.

## Running roms
You must pass the absolute path of the rom as a command line arguement
```
./main.out "ROM PATH"
```


## Built With

* [SFML](https://www.sfml-dev.org/learn.php) - C++ graphics library
. 
## Lisence

* Lisenced with GPL. See Lisence.md.

## Authors

* **Joni Suominen** - *Initial work* - [Joni Suominen](https://github.com/JoniSuominen)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## Acknowledgments

* http://reddit.com/r/emudev
* http://bgb.bircd.org/pandocs.htm
* http://www.codeslinger.co.uk/pages/projects/gameboy.html
* http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-The-CPU
