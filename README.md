# Koivisto UCI

<h1 align="center">
 <img src="https://image.prntscr.com/image/bo9NE9KfRFGPIa_oGP-Q7Q.png" />
</h1>


Koivisto is a strong chess engine written primarily by Kim Kåhre and Finn Eggers in c++. Koivisto in itself is not a complete chess program and requires a UCI-compatible graphical user interface.

### Supported UCI settings:
- Hash
- SyzygyPath (up to 6 pieces)
- Threads (up to 256)

### Acknowledgements
All the Koivisto contributors, [kz04px](https://github.com/kz04px), [Eugenio Bruno](https://github.com/Eugenio-Bruno), [Jay Honnold](https://github.com/jhonnold), [Daniel Dugovic](https://github.com/ddugovic), [Aryan Parekh](https://github.com/Aryan1508/Bit-Genie). Additionally we have recieved invaluable help and advice from [Andrew Grant](https://github.com/AndyGrant/Ethereal) and [theo77186](https://github.com/theo77186). We use [Fathom](https://github.com/jdart1/Fathom) for tablebase probing. [Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page) has been a very usefull resource.

Additionaly, we have recieved support from:
- [Noobpwnftw](https://github.com/noobpwnftw)
- [Mk-Chan](https://github.com/Mk-Chan)
- [Terje Kirstihagen](https://github.com/TerjeKir)
- [bobsquared](https://github.com/bobsquared/Mr_Bob_Chess)




# Compiling


Note that compiler warnings might pop up which can be safely ignored and will most likely be fixed in 
one of the future releases.

## Windows / Linux


We do provide binaries for Windows / Linux systems. You can download them for each release after Koivisto 3.0 [here](https://github.com/Luecx/Koivisto/releases). 
Note that we **strongly recommend** that you build the binaries yourself for best performance.
Assuming build tools have been installed, one can type:

```
git clone https://github.com/Luecx/Koivisto.git
cd Koivisto/

mkdir bin/ 
cd src_files
make native
```

Besides compiling a native version which should be best in terms of performance, one can also compile static executables using:
```
mkdir bin/ 
cd src_files
make release
```

#### MacOS

We do not provide binaries for MacOS yet. 





