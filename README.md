<h1>
C++ Ascii Interpretation of the Image with GUI using ImGui
</h1>

Convert your waifu.image to cool & colourful ascii interpretation!

<img src="documents/window_demo.gif">

Supports resolution resizing, aspect ratio adjustment, and gamma correction.
<hr>
Build

  - cmake . -B <build_dir_path>
  - or enter build dir and cmake ..

Cmake works only for linux at this time since some dependencies fails to connect on mingw environment in linux.
<hr>
Dependencies

- [ImGui](https://github.com/ocornut/imgui) included
- [stb](https://github.com/nothings/stb) included
- glfw3
- xorg
<hr>
Fonts

- [OpenSans](https://fonts.google.com/specimen/Open+Sans) - Window
- [GNU Unifont](https://unifoundry.com/unifont/index.html) - Ascii Rendering

You can use custom fonts in .ttf format
<hr>
Sample Outputs

<img src="documents/lucy-tech.png">
Based on artwork: "Cyberpunk: Edgerunners" Original Lucy Character Design
<br><br>

<img src="documents/makima.png">
Based on artwork done by Whoami https://x.com/sok_0000
<br><br>

<img src="documents/yuno-tech.png">
Based on artwork: "Future Diary" Original Anime Art Cutout
<br><br>

