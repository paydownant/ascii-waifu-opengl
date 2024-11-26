C++ Gui Test using ImGui

Build dir is included just for the sake of example. In applications, build dir needs to be cleaned and restarted using command:
  - cmake . -B <build_dir_path>
  - or enter build dir and cmake ..

Cmake works for only for linux at this time since some dependencies fails to connect on mingw environment in linux.

Dependencies: For opengl & opengl example
  - glfw3 (apt install libglfw3 and libglfw3-dev)
  - xorg (apt install xorg-dev)

Convert your waifu.image to ascii and output to your desired file stream!

Supports ascii resolution resizing, aspect ratio adjustment, and gamma correction.

```
Read: ../images/image.png
Loaded 300 x 268 Image
Created 88 x 31 Buffer
           i!:..............................     ..........!!ii!iiillwwwwwl             
         ii:::.............................          .... .:!i!ii!iiiwiwwiwi            
         i!:..........................................     ::!!!!i!!!iiiiiii            
        i!::............................................. ..::!!!::!!!!!!iii            
       ii::..............................................:..:::!!!::!!!!!!!i!           
      ii!:...........i!i:...............:i....:.:.........:::::!!!:::!!!:!!i!!          
      i!:.....i......:iii...............!ii....i.:.........::::!!!::::!!::!::i          
      i!:.:...!:i...:iiii...........i:.iiii:...ii:.....:....:::!!!:::.:::...: !!i       
      !!:.....iii:..!iiiii!........!!:iiiiii..iiii.:i:......:::!!!:::.:.   ...:!        
      !!......iiii . .:iiiii...!..ii:i:.....  ...............::!!!::::.   ...!          
      !!......iiii:!ll:.iiiii..i.iiiiiill........i:iii.:....:::!!!:::::.:::!!!iii       
      !!......!iiiii!iiliiiii!::iiiiiiiiilli:!wl!iiiii.......:!!!:::....:::::::!!!illl  
      i!.....iiiiiiiii:!!iiiii!iiiiiiiiiiiiiiiiii!iii!......::!!!i.:::..:::::::!        
      !!:...ii!iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii!i:.......:!:!.::::!  :::!!  iii     
       !:...iii:!iiiiiiiiiiiiiiiii!i!iiiiii!iiiiii!i.........:::!::::!    !:::i         
        !:.:iii!:iiiiiiiiiiiiiiiiiiiiiiiiiiiiiilwii:.i.........::::::!     !::!ii       
        !:.. .iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiwiii:ii:........:!::.::!      !::!i       
         !::...:iiiiiiiiiiiiiiiiiiiiiiiiiiiiwwiiiiii.........::!:::::!       !::!il     
          !:: :..!iiiiiiiiiiiiiiiiiiiiiiiiiwwiiii!i.........::! :::::!        !::ii     
           !!: ::.::iiiiiiiiiiiiiiiiiiiiiiww!iiiiii......::::!   :::::!       :::!ii    
             !  :::: :iiiiiiiiiiiiiiiiiiiiwiiiiii: .....:: !i    !::::i       ::::ii    
                 :::   !:!iiiiiiiiiiiiiiiw:!..      ..::: !i     !:::::!      ::::ii    
                  !:   ::...!iiiiiiiiiiii!.        ...:  i        !:::::!    :::::ii    
                   :    :....:iiiiiiiiiiw.        ...::::          ::::::!  ::.:::!i    
               !::......     !iiiiiiiiilw        .   ...::::::!i    i!:::::!:::!!ii     
             !.              !iiiiiiiiiw..                ....::!      !::::::!ii       
           :.               !!iiiiiiiiww.                    ..::!!         i           
           .               !!!i!iiiiiiw!.                    ..:::!                     
         !.                !!!iiiiiiiil.                    ..::::!                     
         .                !!!!iiiiiiiww.                   ...:::!!                     
        :.               .!!!iiiiiiiiww.                  ....::::                      
```