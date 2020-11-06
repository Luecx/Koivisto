# Koivisto UCI

<h1 align="center">
 <img src="https://image.prntscr.com/image/bo9NE9KfRFGPIa_oGP-Q7Q.png" />
</h1>


Koivisto is an AB-engine for chess written in C++ by Kim Kåhre, Finn Eggers and Eugenio Bruno.

### Supported UCI settings:
- Hash
- SyzygyPath (up to 6 pieces)
- Threads (up to 32)

### Acknowledgements
- [Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page)
- [Andrew Grant](https://github.com/AndyGrant/Ethereal)
- [Fathom](https://github.com/jdart1/Fathom)
- [Mk-Chan](https://github.com/Mk-Chan)
- [Terje Kirstihagen](https://github.com/TerjeKir)
- [kz04px](https://github.com/kz04px)
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





