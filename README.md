# Koivisto UCI 1.0

Koivisto is an AB-engine for chess written in C++ by Kim Kahre and Finn Eggers. The strength is estimated to be around 2860 CCRL on a single core.

### Supported UCI settings:
- Hash
- SyzygyPath (up to 6 pieces)
- Threads (up to 8)

### Acknowledgements
- [Eugenio Bruno](https://github.com/Eugenio-Bruno)
- [Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page)
- [Andrew Grant](https://github.com/AndyGrant/Ethereal)
- [Fathom](https://github.com/jdart1/Fathom)
- [Mk-Chan](https://github.com/Mk-Chan)
- [Weiss](https://github.com/TerjeKir)

### Compiling

```
cd src_files
make
```

A few compiler warnings about unused functions might pop up. Those functions are only from the syzygy code which have been copied from Fathom



