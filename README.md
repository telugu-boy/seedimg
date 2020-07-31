# seedimg
~~This currently requires libpng to be installed with vcpkg on Windows.~~

~~libpng, libjpeg (x86. will not work if project is set to x64) is automatically installed through nuget. All contributors are must use nuget for image format libraries until cmake is set up.~~

~~libwebp must be installed through vcpkg as an up to date version is not in Nuget.~~

Please install all libraries via vcpkg, to allow for easier transfer to other IDEs.

Linux: vcpkg install libpng:x86-linux libjpeg-turbo:x86-linux libwebp:x86-linux

Windows: vcpkg install libpng:x86-windows libjpeg-turbo:x86-windows libwebp:x86-windows
