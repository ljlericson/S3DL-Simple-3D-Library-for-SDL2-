  COPYRIGHT (C) Linus Ericson 2024
  Email: ljlericson@protonmail.com

  this project is licensed by the creative commons attribution
  non-commercial-noderivatrives 4.0 international license

  License: CC BY-NC-ND 4.0
  License link: https://creativecommons.org/licenses/by-nc-nd/4.0/deed.en

  NOTICES: 
  1. namespace and all emuns are named under 'S3DL' (yes uppercase)
  2. S3DL uses the SDL2 libraries as well as GLEW/Opengl and cannot work without them installed
     SDL2 libraries include:
      - SDL2 
      - SDL2 image
      - SDL2 mixer
      - SDL2 net
      GLEW must also be installed for efficient rendering
      (performance absouloutly sucks otherwise)
  3. I hope you have realised by this point but this is a C++ library, this
     library does not work natively with C, nor do I intend to release a C
     port going into the future, sorry for any inconveiniance (not really)

  please visit (s3dl.com) for more info or (s3dl.com/wiki.html) for the 
  wiki page (if the website is down I may not have made them yet haha)

  compile command (works with majority of compilers):
  g++ example.cpp -lmingw32 -lSDL2 -llibSDL2_image -llibSDL2_net -llibSDL2_ttf -o example.exe
  (replace example with you project name)
