~~~~~~~~~~~~~~~~~~~~~ README : mSquared File Transfer ~~~~~~~~~~~~~~~~~~~~~

                                Developed by: 
                           Mrinal Dhar 201325118
                        Mounika Somisetty 201330076


A multi-purpose file transfer program with chat capabilities, an easy and 
intuitive web based user interface and a powerful command line interface!


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Instructions to Compile: 

    Just write ./run.sh on the terminal and it will generate an 
    executable called server in the same directory.

===========================================================================

Instructions to Run:
    
    Run the server using ./server in the terminal.
    This automatically starts a TCP server at port 5005 and a web
    server at port 8000 (for the web GUI :P). 
    The default IP address is localhost (127.0.0.1), and this can be 
    changed by giving a command line argument to the server file.

    For example, to connect to another computer at 10.1.39.10, we use:
        ./server 10.1.39.10

    Once the mSquared server is running, a prompt appears on the screen:

        $>

============================================================================

To perform various tasks in CLI, use this prompt. 

    For example,

    IndexGet: (stores output in a index.txt_TCP file)
        $> IndexGet --longlist
        $> IndexGet --shortlist
        $> IndexGet --regex serve

    FileDownload: 
        $> FileDownload TCP test
        $> FileDownload UDP test

    FileUpload:
        $> FileUpload TCP test
        $> FileUpload UDP test

    FileHash: (stores output in a fileHashOutput.txt_TCP file)
        $> FileHash --verify test
        $> FileHash --checkall

    Send chat message:
        $> -Hello

    See this help screen:
        $> help

    Exit:
        $> exit

===========================================================================

To access the web interface, open up a browser and point it to 
    http://localhost:8000


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                 Copyright (C) 2015 - All Rights Reserved

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
