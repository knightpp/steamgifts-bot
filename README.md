[![Snap Status](https://build.snapcraft.io/badge/knightpp/steamgifts-bot.svg)](https://build.snapcraft.io/user/knightpp/steamgifts-bot)

# Configure and run
* Login steamgifts.com
* Obtain PHPSSESID cookie from the site (Firefox: F12 -> Storage -> Cookies)
* Open terminal and run: `steamgiftsbot --cookie COOKIE_HERE`

## Snap
```
git clone https://github.com/knightpp/steamgifts-bot
cd steamgifts-bot
snapcraft
```
## Ubuntu
Install dependencies
```
    sudo apt install libhtmlcxx-dev
```
and one of
```
    libcurl4-openssl-dev
    libcurl4-nss-dev
    libcurl4-gnutls-dev
```
Clone and build cmake
```
git clone https://github.com/knightpp/steamgifts-bot
cd steamgifts-bot
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j3
```
## Windows (x64 only prebuilt deps)
Open MSVC developer CMD. Clone and build cmake
```
git clone https://github.com/knightpp/steamgifts-bot
cd steamgifts-bot
mkdir build && cd build
cmake .. -A x64
cmake --build . --config Release
```
## TODO
* flatpak