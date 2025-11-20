#include "../includes/Channel.hpp"

Channel::Channel() {};
Channel::~Channel() {};
Channel::Channel(const Channel &type) {};
Channel &Channel:: operator=(const Channel &type1) {};

//Add Member
//Step 1: Get client and add it to the map

//Remove Member
//Step 1: Check if client exist in the channel or not
//Step 2: Remove client from the map

//Set Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: Add member onto the vector 
//Step 3: Set the status in the client class

//Unset Operator Privilage
//Step 1: Check if user is already a operator and is already a member
//Step 2: remove member in the vector 
//Step 3: Set the status in the client class