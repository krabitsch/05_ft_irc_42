  // Commands_Mode.cpp     // MODE/INVITE/KICK (subset)
  #include "../includes/Channel.hpp"


  //Mode - Change the Channels Mode
  //Flags
  //- i: Set/remove the invite only 
  //- t: Set/remove the restrictions of the TOPIC command to the channel operators
  //- k: Set/remove the channel key (password) //basically remove the password access
  //- o: Give/tkae channel operator privilege
  //- l: Set/remove the user limit to channel 

  //Invite - Invite a client of the current channel
  //Step 1:Use either the client nickname or username 
  //Step 2:Enter a loop until the user either accepts the invite or rejects it
  //Step 3:If client accepts it add to client that he now has access to the channel, and add the user into the channel


  //KICK - Ejecting a client from the channel
  //The Kick interacts with the channel, with no channel you are unable to kick
  //Step 1: First we need if the client & Channel & Operator exists
  //Step 2: Check if they are all apart of the channel and have access to it, 
  //Step 3: Declare in the channel that they are getting kick simple std::cout message
  //Step 4: Remove the user from the channel 

  //Questions to Ask: Should an operator be able to kick another operator?
  
